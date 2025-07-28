#pragma once

#include "CauldronZero/Macros.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

class Actor;
class HitResult;
class CompoundTag;

namespace CauldronZero::event {

class ProjectileHitBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Actor&           mOwner;
    HitResult const& mHitResult;

public:
    constexpr explicit ProjectileHitBeforeEvent(Actor& owner, HitResult const& hitResult)
    : mOwner(owner),
      mHitResult(hitResult) {}

    CZ_API Actor&           getOwner() const;
    CZ_API HitResult const& getHitResult() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

class ProjectileHitAfterEvent final : public ll::event::Event {
protected:
    Actor&           mOwner;
    HitResult const& mHitResult;

public:
    constexpr explicit ProjectileHitAfterEvent(Actor& owner, HitResult const& hitResult)
    : mOwner(owner),
      mHitResult(hitResult) {}

    CZ_API Actor&           getOwner() const;
    CZ_API HitResult const& getHitResult() const;

    virtual void serialize(CompoundTag& nbt) const override;
};


void registerProjectileHitEventHooks();
void unregisterProjectileHitEventHooks();

} // namespace CauldronZero::event
