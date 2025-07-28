#include "CauldronZero/events/world/block/FarmTrampleEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/FarmBlock.h"

namespace CauldronZero::event {

// --- 耕地踩踏前事件 ---
BlockSource&    FarmTrampleBeforeEvent::getRegion() const { return mRegion; }
const BlockPos& FarmTrampleBeforeEvent::getPos() const { return mPos; }
Actor*          FarmTrampleBeforeEvent::getActor() const { return mActor; }
float           FarmTrampleBeforeEvent::getFallDistance() const { return mFallDistance; }

void FarmTrampleBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["dimension"]    = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]          = ListTag{mPos.x, mPos.y, mPos.z};
    nbt["actor"]        = ll::event::serializeRefObj(*mActor);
    nbt["fallDistance"] = mFallDistance;
}

class FarmTrampleBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, FarmTrampleBeforeEvent> {};

// --- 耕地踩踏后事件 ---
BlockSource&    FarmTrampleAfterEvent::getRegion() const { return mRegion; }
const BlockPos& FarmTrampleAfterEvent::getPos() const { return mPos; }
Actor*          FarmTrampleAfterEvent::getActor() const { return mActor; }
float           FarmTrampleAfterEvent::getFallDistance() const { return mFallDistance; }

void FarmTrampleAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["dimension"]    = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]          = ListTag{mPos.x, mPos.y, mPos.z};
    nbt["actor"]        = ll::event::serializeRefObj(*mActor);
    nbt["fallDistance"] = mFallDistance;
}

class FarmTrampleAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, FarmTrampleAfterEvent> {};


// --- 挂钩 ---
LL_TYPE_INSTANCE_HOOK(
    FarmTrampleEventHook,
    ll::memory::HookPriority::Normal,
    FarmBlock,
    &FarmBlock::$transformOnFall,
    void,
    ::BlockSource&    region,
    ::BlockPos const& pos,
    ::Actor*          actor,
    float             fallDistance
) {
    try {
        auto beforeEvent = FarmTrampleBeforeEvent(region, pos, actor, fallDistance);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(region, pos, actor, fallDistance);

        auto afterEvent = FarmTrampleAfterEvent(region, pos, actor, fallDistance);
        ll::event::EventBus::getInstance().publish(afterEvent);

    } catch (const SEH_Exception& e) {
        logger.error(
            "在 FarmTrampleEventHook 中捕获到 SEH 异常: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(region, pos, actor, fallDistance);
        throw;
    } catch (const std::exception& e) {
        logger.error("在 FarmTrampleEventHook 中捕获到标准异常: {}", e.what());
        origin(region, pos, actor, fallDistance);
        throw;
    } catch (...) {
        logger.error("在 FarmTrampleEventHook 中捕获到未知异常");
        origin(region, pos, actor, fallDistance);
        throw;
    }
}

void registerFarmTrampleEventHooks() { FarmTrampleEventHook::hook(); }
void unregisterFarmTrampleEventHooks() { FarmTrampleEventHook::unhook(); }

} // namespace CauldronZero::event
