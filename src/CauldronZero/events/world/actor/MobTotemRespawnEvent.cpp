#include "MobTotemRespawnEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/actor/ActorDamageSource.h"
#include "mc/world/actor/Mob.h"

namespace CauldronZero::event {

// --- MobTotemRespawnBeforeEvent ---
Mob&                     MobTotemRespawnBeforeEvent::getMob() const { return mMob; }
ActorDamageSource const& MobTotemRespawnBeforeEvent::getSource() const { return mSource; }

void MobTotemRespawnBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["mob"]    = ll::event::serializeRefObj(mMob);
    nbt["source"] = ll::event::serializeRefObj(mSource);
}

class MobTotemRespawnBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, MobTotemRespawnBeforeEvent> {};


// --- MobTotemRespawnAfterEvent ---
Mob&                     MobTotemRespawnAfterEvent::getMob() const { return mMob; }
ActorDamageSource const& MobTotemRespawnAfterEvent::getSource() const { return mSource; }

void MobTotemRespawnAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["mob"]    = ll::event::serializeRefObj(mMob);
    nbt["source"] = ll::event::serializeRefObj(mSource);
}

class MobTotemRespawnAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, MobTotemRespawnAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    MobTotemRespawnHook,
    ll::memory::HookPriority::Normal,
    Mob,
    &Mob::checkTotemDeathProtection,
    bool,
    ::ActorDamageSource const& source
) {
    try {
        auto& mob   = *this;
        auto  event = MobTotemRespawnBeforeEvent(mob, source);
        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return false;
        }

        bool const activated = origin(source);

        if (activated) {
            auto afterEvent = MobTotemRespawnAfterEvent(mob, source);
            ll::event::EventBus::getInstance().publish(afterEvent);
        }

        return activated;
    } catch (const SEH_Exception& e) {
        logger.warn(
            "MobTotemRespawnHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(source);
    } catch (const std::exception& e) {
        logger.warn("MobTotemRespawnHook 发生 C++ 异常: {}", e.what());
        return origin(source);
    } catch (...) {
        logger.warn("MobTotemRespawnHook 发生未知异常！");
        return origin(source);
    }
}

void registerMobTotemRespawnEventHooks() { MobTotemRespawnHook::hook(); }
void unregisterMobTotemRespawnEventHooks() { MobTotemRespawnHook::unhook(); }

} // namespace CauldronZero::event