#include "ProjectileCreateEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/memory/Hook.h"
#include "mc/entity/components_json_legacy/ProjectileComponent.h"
#include "mc/world/actor/Actor.h"
#include <ll/api/event/EventRefObjSerializer.h>

namespace CauldronZero::event {

Actor& ProjectileCreateBeforeEvent::getShooter() const { return mShooter; }
ProjectileComponent* ProjectileCreateBeforeEvent::getProjectile() const { return mProjectile; }
Vec3&  ProjectileCreateBeforeEvent::getDirection() { return mDirection; }
float& ProjectileCreateBeforeEvent::getPower() { return mPower; }
float& ProjectileCreateBeforeEvent::getUncertainty() { return mUncertainty; }
Vec3&  ProjectileCreateBeforeEvent::getBaseSpeed() { return mBaseSpeed; }
Actor* ProjectileCreateBeforeEvent::getTarget() const { return mTarget; }


void ProjectileCreateBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["shooter"]     = ll::event::serializeRefObj(mShooter);
    nbt["projectile"]  = ll::event::serializeRefObj(mProjectile);
    nbt["direction"]   = mDirection.toString();
    nbt["power"]       = mPower;
    nbt["uncertainty"] = mUncertainty;
    nbt["baseSpeed"]   = mBaseSpeed.toString();
    if (mTarget) {
        nbt["target"] = ll::event::serializeRefObj(*mTarget);
    }
}

Actor& ProjectileCreateAfterEvent::getShooter() const { return mShooter; }
ProjectileComponent* ProjectileCreateAfterEvent::getProjectile() const { return mProjectile; }

void ProjectileCreateAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["shooter"]    = ll::event::serializeRefObj(mShooter);
    nbt["projectile"] = ll::event::serializeRefObj(mProjectile);
}


LL_TYPE_INSTANCE_HOOK(
    ProjectileCreateEventHook,
    HookPriority::Normal,
    ProjectileComponent,
    &ProjectileComponent::shoot,
    void,
    ::Actor&           owner,
    ::Vec3 const&      dir,
    float              pow,
    float              uncertainty,
    ::Vec3 const&      baseSpeed,
    ::Actor*           target
) {
    try {
        auto projectile = this;
        auto  event =
            ProjectileCreateBeforeEvent(owner, projectile, dir, pow, uncertainty, baseSpeed, target);
        ll::event::EventBus::getInstance().publish(event);
        if (event.isCancelled()) {
            return;
        }
        origin(
            owner,
            event.getDirection(),
            event.getPower(),
            event.getUncertainty(),
            event.getBaseSpeed(),
            target
        );
        auto afterEvent = ProjectileCreateAfterEvent(owner, projectile);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 ProjectileCreateEventHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(owner, dir, pow, uncertainty, baseSpeed, target);
    } catch (const std::exception& e) {
        logger.error("标准异常在 ProjectileCreateEventHook::hook 中捕获: {}", e.what());
        origin(owner, dir, pow, uncertainty, baseSpeed, target);
    } catch (...) {
        logger.error("未知异常在 ProjectileCreateEventHook::hook 中捕获");
        origin(owner, dir, pow, uncertainty, baseSpeed, target);
    }
}

void registerProjectileCreateHooks() { ProjectileCreateEventHook::hook(); }

void unregisterProjectileCreateHooks() { ProjectileCreateEventHook::unhook(); }

} // namespace CauldronZero::event