#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Mob;
class ActorDamageSource;

namespace CauldronZero::event {

// 在生物被图腾复活前触发。
// 此事件可被取消，以阻止复活。
class MobTotemRespawnBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Mob&                     mMob;
    ActorDamageSource const& mSource;

public:
    constexpr explicit MobTotemRespawnBeforeEvent(Mob& mob, ActorDamageSource const& source)
    : mMob(mob),
      mSource(source) {}

    CZ_API Mob&                     getMob() const;
    CZ_API ActorDamageSource const& getSource() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 在生物被图腾复活后触发。
class MobTotemRespawnAfterEvent final : public ll::event::Event {
protected:
    Mob&                     mMob;
    ActorDamageSource const& mSource;

public:
    constexpr explicit MobTotemRespawnAfterEvent(Mob& mob, ActorDamageSource const& source)
    : mMob(mob),
      mSource(source) {}

    CZ_API Mob&                     getMob() const;
    CZ_API ActorDamageSource const& getSource() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerMobTotemRespawnEventHooks();
void unregisterMobTotemRespawnEventHooks();

} // namespace CauldronZero::event