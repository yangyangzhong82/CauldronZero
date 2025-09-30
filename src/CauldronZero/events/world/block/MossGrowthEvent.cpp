#include "CauldronZero/events/world/block/MossGrowthEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/WorldBlockTarget.h"
#include "mc/world/level/levelgen/feature/VegetationPatchFeature.h"
#include "mc/world/level/BlockPos.h" // 确保包含 BlockPos 的定义
#include <memory> // For std::make_unique

namespace CauldronZero::event {

// --- MossGrowthBeforeEvent ---
IBlockWorldGenAPI& MossGrowthBeforeEvent::getTarget() const { return mTarget; }
Random&            MossGrowthBeforeEvent::getRandom() const { return mRandom; }
const BlockPos&    MossGrowthBeforeEvent::getOrigin() const { return mOrigin; }
int                MossGrowthBeforeEvent::getXRadius() const { return mXRadius; }
int                MossGrowthBeforeEvent::getZRadius() const { return mZRadius; }

void MossGrowthBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);

    nbt["origin"]   = ListTag{mOrigin.x, mOrigin.y, mOrigin.z};
    nbt["xRadius"]  = mXRadius;
    nbt["zRadius"]  = mZRadius;
}

class MossGrowthBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, MossGrowthBeforeEvent> {};

// --- MossGrowthAfterEvent ---
IBlockWorldGenAPI&    MossGrowthAfterEvent::getTarget() const { return mTarget; }
Random&               MossGrowthAfterEvent::getRandom() const { return mRandom; }
const BlockPos&       MossGrowthAfterEvent::getOrigin() const { return mOrigin; }
int                   MossGrowthAfterEvent::getXRadius() const { return mXRadius; }
int                   MossGrowthAfterEvent::getZRadius() const { return mZRadius; }
std::vector<BlockPos> MossGrowthAfterEvent::getResult() const { return mResult; }

void MossGrowthAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);

    nbt["origin"]   = ListTag{mOrigin.x, mOrigin.y, mOrigin.z};
    nbt["xRadius"]  = mXRadius;
    nbt["zRadius"]  = mZRadius;

}

class MossGrowthAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, MossGrowthAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    MossGrowthEventHook,
    HookPriority::Normal,
    VegetationPatchFeature,
    &VegetationPatchFeature::_placeGroundPatch,
    std::vector<BlockPos>,
    ::IBlockWorldGenAPI& target,
    ::Random&            random,
    ::BlockPos const&    patchOrigin,
    int                  xRadius,
    int                  zRadius
) {
    try {
        auto beforeEvent = MossGrowthBeforeEvent(target, random, patchOrigin, xRadius, zRadius);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return {}; 
        }

        std::vector<BlockPos> result = origin(target, random, patchOrigin, xRadius, zRadius);

        auto afterEvent = MossGrowthAfterEvent(target, random, patchOrigin, xRadius, zRadius, result);
        ll::event::EventBus::getInstance().publish(afterEvent);

        return result;
    } catch (const SEH_Exception& e) {
        logger.error(
            "在 MossGrowthEventHook 中捕获到 SEH 异常: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        return origin(target, random, patchOrigin, xRadius, zRadius);
    } catch (const std::exception& e) {
        logger.error("在 MossGrowthEventHook 中捕获到标准异常: {}", e.what());
        return origin(target, random, patchOrigin, xRadius, zRadius);
    } catch (...) {
        logger.error("在 MossGrowthEventHook 中捕获到未知异常");
        return origin(target, random, patchOrigin, xRadius, zRadius);
    }
}

void registerMossGrowthEventHooks() { MossGrowthEventHook::hook(); }
void unregisterMossGrowthEventHooks() { MossGrowthEventHook::unhook(); }

} // namespace CauldronZero::event
