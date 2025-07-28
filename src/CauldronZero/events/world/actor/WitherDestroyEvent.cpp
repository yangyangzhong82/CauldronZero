#include "WitherDestroyEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/actor/boss/WitherBoss.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/phys/AABB.h"

namespace CauldronZero::event {

// --- WitherDestroyBeforeEvent ---
WitherBoss&                  WitherDestroyBeforeEvent::getWither() const { return mWither; }
Level&                       WitherDestroyBeforeEvent::getLevel() const { return mLevel; }
AABB const&                  WitherDestroyBeforeEvent::getAABB() const { return mAABB; }
BlockSource&                 WitherDestroyBeforeEvent::getRegion() const { return mRegion; }
int&                         WitherDestroyBeforeEvent::getRange() const { return mRange; }
WitherBoss::WitherAttackType WitherDestroyBeforeEvent::getAttackType() const { return mAttackType; }

void WitherDestroyBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["wither"]     = ll::event::serializeRefObj(mWither);
    nbt["level"]      = ll::event::serializeRefObj(mLevel);
    nbt["aabb_min_x"] = mAABB.min.x;
    nbt["aabb_min_y"] = mAABB.min.y;
    nbt["aabb_min_z"] = mAABB.min.z;
    nbt["aabb_max_x"] = mAABB.max.x;
    nbt["aabb_max_y"] = mAABB.max.y;
    nbt["aabb_max_z"] = mAABB.max.z;
    nbt["region"]     = ll::event::serializeRefObj(mRegion);
    nbt["range"]      = mRange;
    nbt["attackType"] = static_cast<int>(mAttackType);
}

// --- WitherDestroyAfterEvent ---
WitherBoss&                  WitherDestroyAfterEvent::getWither() const { return mWither; }
Level&                       WitherDestroyAfterEvent::getLevel() const { return mLevel; }
AABB const&                  WitherDestroyAfterEvent::getAABB() const { return mAABB; }
BlockSource&                 WitherDestroyAfterEvent::getRegion() const { return mRegion; }
int                          WitherDestroyAfterEvent::getRange() const { return mRange; }
WitherBoss::WitherAttackType WitherDestroyAfterEvent::getAttackType() const { return mAttackType; }

void WitherDestroyAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["wither"]     = ll::event::serializeRefObj(mWither);
    nbt["level"]      = ll::event::serializeRefObj(mLevel);
    nbt["aabb_min_x"] = mAABB.min.x;
    nbt["aabb_min_y"] = mAABB.min.y;
    nbt["aabb_min_z"] = mAABB.min.z;
    nbt["aabb_max_x"] = mAABB.max.x;
    nbt["aabb_max_y"] = mAABB.max.y;
    nbt["aabb_max_z"] = mAABB.max.z;
    nbt["region"]     = ll::event::serializeRefObj(mRegion);
    nbt["range"]      = mRange;
    nbt["attackType"] = static_cast<int>(mAttackType);
}

class WitherDestroyBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, WitherDestroyBeforeEvent> {};

class WitherDestroyAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, WitherDestroyAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    WitherDestroyEventHook,
    ll::memory::HookPriority::Normal,
    WitherBoss,
    &WitherBoss::_destroyBlocks,
    void,
    ::Level&                       level,
    ::AABB const&                  bb,
    ::BlockSource&                 region,
    int                            range,
    ::WitherBoss::WitherAttackType attackType
) {
    try {
        auto& wither = *this;
        auto  event  = WitherDestroyBeforeEvent(wither, level, bb, region, range, attackType);
        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return;
        }

        origin(level, bb, region, range, attackType);

        auto afterEvent = WitherDestroyAfterEvent(wither, level, bb, region, range, attackType);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "WitherDestroyEventHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(level, bb, region, range, attackType);
    } catch (const std::exception& e) {
        logger.warn("WitherDestroyEventHook 发生 C++ 异常: {}", e.what());
        origin(level, bb, region, range, attackType);
    } catch (...) {
        logger.warn("WitherDestroyEventHook 发生未知异常！");
        origin(level, bb, region, range, attackType);
    }
}

void registerWitherDestroyEventHooks() { WitherDestroyEventHook::hook(); }
void unregisterWitherDestroyEventHooks() { WitherDestroyEventHook::unhook(); }

} // namespace CauldronZero::event
