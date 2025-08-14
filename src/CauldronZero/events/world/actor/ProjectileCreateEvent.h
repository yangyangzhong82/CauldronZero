#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/deps/core/math/Vec3.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Actor;
class ProjectileComponent;
namespace CauldronZero::event {

class ProjectileCreateBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Actor&      mShooter;
    ProjectileComponent*      mProjectile;
    Vec3        mDirection;
    float       mPower;
    float       mUncertainty;
    Vec3        mBaseSpeed;
    Actor*      mTarget;

public:
    constexpr explicit ProjectileCreateBeforeEvent(
        Actor& shooter,
        ProjectileComponent* projectile,
        Vec3   direction,
        float  power,
        float  uncertainty,
        Vec3   baseSpeed,
        Actor* target
    )
    : mShooter(shooter),
      mProjectile(projectile),
      mDirection(direction),
      mPower(power),
      mUncertainty(uncertainty),
      mBaseSpeed(baseSpeed),
      mTarget(target) {}

    CZ_API Actor& getShooter() const;
    CZ_API ProjectileComponent* getProjectile() const;
    CZ_API Vec3& getDirection();
    CZ_API float& getPower();
    CZ_API float& getUncertainty();
    CZ_API Vec3& getBaseSpeed();
    CZ_API Actor* getTarget() const;

    void serialize(CompoundTag& nbt) const override;
};

class ProjectileCreateAfterEvent final : public ll::event::Event {
protected:
    Actor& mShooter;
    ProjectileComponent* mProjectile;

public:
    constexpr explicit ProjectileCreateAfterEvent(Actor& shooter, ProjectileComponent* projectile)
    : mShooter(shooter),
      mProjectile(projectile) {}

    CZ_API Actor& getShooter() const;
    CZ_API ProjectileComponent* getProjectile() const;

    void serialize(CompoundTag& nbt) const override;
};

void registerProjectileCreateHooks();
void unregisterProjectileCreateHooks();

} // namespace CauldronZero::event