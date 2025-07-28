#include "MobHurtEffectEvent.h"
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
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/Level.h"


namespace CauldronZero::event {

Mob&                     MobHurtEffectEvent::getMob() const { return mMob; }
ActorDamageSource const& MobHurtEffectEvent::getSource() const { return mSource; }
float&                   MobHurtEffectEvent::getDamage() const { return mDamage; }
Actor*                   MobHurtEffectEvent::getDamageSource() const { return mDamageSource; }


void MobHurtEffectEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["mob"]    = ll::event::serializeRefObj(mMob);
    nbt["source"] = ll::event::serializeRefObj(mSource);
    nbt["damage"] = mDamage;
    if (mDamageSource) {
        nbt["damageSource"] = ll::event::serializeRefObj(*mDamageSource);
    }
}

class MobHurtEffectEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::MobHurtEffectEvent> {};


LL_TYPE_INSTANCE_HOOK(
    MobHurtEffectHook,
    ll::memory::HookPriority::Normal,
    Mob,
    &Mob::getDamageAfterResistanceEffect,
    float,
    ::ActorDamageSource const& source,
    float                      damage
) {
    try {
        Actor* damageSource = nullptr;
        if (source.isEntitySource()) {
            if (source.isChildEntitySource()) {
                damageSource = ll::service::getLevel()->fetchEntity(source.getEntityUniqueID(), false);
            } else {
                damageSource = ll::service::getLevel()->fetchEntity(source.getDamagingEntityUniqueID(), false);
            }
        }

        auto& mob   = *this;
        auto  event = MobHurtEffectEvent(mob, source, damage, damageSource);
        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return 0.0f;
        }

        return origin(source, damage);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "MobHurtEffectHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(source, damage);
    } catch (const std::exception& e) {
        logger.warn("MobHurtEffectHook 发生 C++ 异常: {}", e.what());
        return origin(source, damage);
    } catch (...) {
        logger.warn("MobHurtEffectHook 发生未知异常！");
        return origin(source, damage);
    }
}

void registerMobHurtEffectEventHooks() { MobHurtEffectHook::hook(); }
void unregisterMobHurtEffectEventHooks() { MobHurtEffectHook::unhook(); }

} // namespace CauldronZero::event
