#pragma once

#include "CauldronZero/Macros.h"
#include "mc/world/actor/ActorDamageSource.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

class Mob;
class CompoundTag;
class Actor;

namespace CauldronZero::event {

class MobHurtEffectEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Mob&                     mMob;
    ActorDamageSource const& mSource;
    float&                   mDamage;
    Actor*                   mDamageSource;

public:
    constexpr explicit MobHurtEffectEvent(
        Mob&                     mob,
        ActorDamageSource const& source,
        float&                   damage,
        Actor*                   damageSource
    )
    : mMob(mob),
      mSource(source),
      mDamage(damage),
      mDamageSource(damageSource) {}

    CZ_API Mob&                     getMob() const;
    CZ_API ActorDamageSource const& getSource() const;
    CZ_API float&                   getDamage() const;
    CZ_API Actor*                   getDamageSource() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerMobHurtEffectEventHooks();
void unregisterMobHurtEffectEventHooks();

} // namespace CauldronZero::event
