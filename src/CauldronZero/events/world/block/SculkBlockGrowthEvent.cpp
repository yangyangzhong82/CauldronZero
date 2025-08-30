#include "SculkBlockGrowthEvent.h"
#include "CauldronZero/Logger.h"
#include "CauldronZero/SEHHandler.h"
#include "ll/api/memory/Hook.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include <mc/world/level/WorldBlockTarget.h>
#include <mc/world/level/block/SculkBlockBehavior.h>
#include "mc/util/Random.h"


namespace CauldronZero::event {

// --- SculkBlockGrowthBeforeEvent ---
IBlockWorldGenAPI& SculkBlockGrowthBeforeEvent::getTarget() const { return mTarget; }
BlockSource* SculkBlockGrowthBeforeEvent::getRegion() const { return mRegion; }
BlockPos const& SculkBlockGrowthBeforeEvent::getPos() const { return mPos; }
Random& SculkBlockGrowthBeforeEvent::getRandom() const { return mRandom; }
SculkSpreader& SculkBlockGrowthBeforeEvent::getSpreader() const { return mSpreader; }

void SculkBlockGrowthBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    // target 和 region 无法直接序列化，因为它们是接口或指针，且没有提供序列化方法
    nbt["pos_x"] = mPos.x;
    nbt["pos_y"] = mPos.y;
    nbt["pos_z"] = mPos.z;
    // Random 也无法直接序列化
    nbt["spreader"] = ll::event::serializeRefObj(mSpreader);
}

class SculkBlockGrowthBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::SculkBlockGrowthBeforeEvent> {};

// --- SculkBlockGrowthAfterEvent ---
IBlockWorldGenAPI& SculkBlockGrowthAfterEvent::getTarget() const { return mTarget; }
BlockSource* SculkBlockGrowthAfterEvent::getRegion() const { return mRegion; }
BlockPos const& SculkBlockGrowthAfterEvent::getPos() const { return mPos; }
Random& SculkBlockGrowthAfterEvent::getRandom() const { return mRandom; }
SculkSpreader& SculkBlockGrowthAfterEvent::getSpreader() const { return mSpreader; }

void SculkBlockGrowthAfterEvent::serialize(CompoundTag& nbt) const {
    // target 和 region 无法直接序列化，因为它们是接口或指针，且没有提供序列化方法
    nbt["pos_x"] = mPos.x;
    nbt["pos_y"] = mPos.y;
    nbt["pos_z"] = mPos.z;
    // Random 也无法直接序列化
    nbt["spreader"] = ll::event::serializeRefObj(mSpreader);
}

class SculkBlockGrowthAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::SculkBlockGrowthAfterEvent> {};


LL_STATIC_HOOK(
    SculkBlockGrowthHook,
    HookPriority::Normal,
    &SculkBlockBehavior::_placeGrowthAt,
    void,
    ::IBlockWorldGenAPI& target,
    ::BlockSource*       region,
    ::BlockPos const&    pos,
    ::Random&            random,
    ::SculkSpreader&     spreader
) {
    try {
        auto beforeEvent = SculkBlockGrowthBeforeEvent(target, region, pos, random, spreader);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(target, region, pos, random, spreader);

        auto afterEvent = SculkBlockGrowthAfterEvent(target, region, pos, random, spreader);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "SculkBlockGrowthHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(target, region, pos, random, spreader);
    } catch (const std::exception& e) {
        logger.warn("SculkBlockGrowthHook 发生 C++ 异常: {}", e.what());
        origin(target, region, pos, random, spreader);
    } catch (...) {
        logger.warn("SculkBlockGrowthHook 发生未知异常！");
        origin(target, region, pos, random, spreader);
    }
}

void registerSculkBlockGrowthEventHooks() { SculkBlockGrowthHook::hook(); }
void unregisterSculkBlockGrowthEventHooks() { SculkBlockGrowthHook::unhook(); }

} // namespace CauldronZero::event
