#include "CauldronZero/events/world/block/DragonEggBlockEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/deps/shared_types/legacy/LevelEvent.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/util/Random.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc\deps\core\math\Random.h"
#include "mc\world\events\gameevents\GameEventRegistry.h"
#include "mc\world\level\block\DragonEggBlock.h"
#include "mc\world\level\block\registry\BlockTypeRegistry.h"
#include <mc/world/events/gameevents/GameEventRegistry.h>
#include <mc/world/level/block/VanillaBlockTypeIds.h>


namespace CauldronZero::event {

// --- DragonEggTeleportBeforeEvent ---
BlockSource&    DragonEggTeleportBeforeEvent::getRegion() const { return mRegion; }
const BlockPos& DragonEggTeleportBeforeEvent::getOldPos() const { return mOldPos; }
BlockPos&       DragonEggTeleportBeforeEvent::getNewPos() const { return mNewPos; }


void DragonEggTeleportBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["dimension"] = static_cast<int>(mRegion.getDimensionId());
    nbt["oldPos"]    = ListTag{mOldPos.x, mOldPos.y, mOldPos.z};
    nbt["newPos"]    = ListTag{mNewPos.x, mNewPos.y, mNewPos.z};
}

class DragonEggTeleportBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, DragonEggTeleportBeforeEvent> {};

// --- DragonEggTeleportAfterEvent ---
BlockSource&    DragonEggTeleportAfterEvent::getRegion() const { return mRegion; }
const BlockPos& DragonEggTeleportAfterEvent::getOldPos() const { return mOldPos; }
const BlockPos& DragonEggTeleportAfterEvent::getNewPos() const { return mNewPos; }

void DragonEggTeleportAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["dimension"] = static_cast<int>(mRegion.getDimensionId());
    nbt["oldPos"]    = ListTag{mOldPos.x, mOldPos.y, mOldPos.z};
    nbt["newPos"]    = ListTag{mNewPos.x, mNewPos.y, mNewPos.z};
}

class DragonEggTeleportAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, DragonEggTeleportAfterEvent> {};


LL_STATIC_HOOK(
    DragonEggBlockTeleportEventHook,
    HookPriority::Low,
    &DragonEggBlock::_attemptTeleport,
    void,
    ::BlockSource&    region,
    ::Random&         random,
    ::BlockPos const& pos
) {
    try {
        auto& level = region.getLevel();
        if (level.isClientSide()) {
            return;
        }
        auto& coreRandom = random.mRandom->mObject;

        BlockPos newPos;
        bool     foundValidPos = false;

        for (int i = 0; i < 1000; ++i) {
            int offsetX = (coreRandom._genRandInt32() & 0xF) - (coreRandom._genRandInt32() & 0xF); // [-15, 15]
            int offsetZ = (coreRandom._genRandInt32() & 0xF) - (coreRandom._genRandInt32() & 0xF); // [-15, 15]
            int offsetY = (coreRandom._genRandInt32() & 7) - (coreRandom._genRandInt32() & 7);     // [-7, 7]

            BlockPos currentAttemptPos = pos;
            currentAttemptPos.x += offsetX;
            currentAttemptPos.y += offsetY;
            currentAttemptPos.z += offsetZ;

            if (region.isEmptyBlock(currentAttemptPos.x, currentAttemptPos.y, currentAttemptPos.z)
                && region.mMaxHeight >= currentAttemptPos.y && region.mMinHeight <= currentAttemptPos.y) {
                newPos        = currentAttemptPos;
                foundValidPos = true;
                break;
            }
        }

        if (!foundValidPos) {
            return; // No valid position found, do nothing.
        }

        auto beforeEvent = DragonEggTeleportBeforeEvent(region, pos, newPos);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        // Use the (potentially modified) newPos from the event
        BlockPos finalNewPos = beforeEvent.getNewPos();

        logger.debug(
            "DragonEggBlockTeleportEvent: Teleporting egg from ({}, {}, {}) to ({}, {}, {})",
            pos.x,
            pos.y,
            pos.z,
            finalNewPos.x,
            finalNewPos.y,
            finalNewPos.z
        );
        region.postGameEvent(nullptr, GameEventRegistry::teleport(), pos, nullptr);
        int diffX = pos.x - finalNewPos.x;
        int diffY = pos.y - finalNewPos.y;
        int diffZ = pos.z - finalNewPos.z;
        int particleData =
            abs(diffZ)
            | ((abs(diffY)
                | (((((diffX >> 31) | (2 * ((diffY >> 31) | (2 * (diffZ >> 31))))) << 8) | abs(diffX)) << 8))
               << 8);

        level.broadcastLocalEvent(region, SharedTypes::Legacy::LevelEvent::ParticlesDragonEgg, pos, particleData);
        region.setBlock(
            finalNewPos,
            BlockTypeRegistry::get().getDefaultBlockState(VanillaBlockTypeIds::DragonEgg(), true),
            3 /* BlockUpdateFlag::All */,
            nullptr,
            nullptr
        );
        region.removeBlock(pos);

        auto afterEvent = DragonEggTeleportAfterEvent(region, pos, finalNewPos);
        ll::event::EventBus::getInstance().publish(afterEvent);

    } catch (const SEH_Exception& e) {
        logger.error(
            "在 DragonEggBlockTeleportEventHook 中捕获到 SEH 异常: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(region, random, pos);
        throw;
    } catch (const std::exception& e) {
        logger.error("在 DragonEggBlockTeleportEventHook 中捕获到标准异常: {}", e.what());
        origin(region, random, pos);
        throw;
    } catch (...) {
        logger.error("在 DragonEggBlockTeleportEventHook 中捕获到未知异常");
        origin(region, random, pos);
        throw;
    }
}

void registerDragonEggBlockEventHooks() { DragonEggBlockTeleportEventHook::hook(); }
void unregisterDragonEggBlockEventHooks() { DragonEggBlockTeleportEventHook::unhook(); }

} // namespace CauldronZero::event
