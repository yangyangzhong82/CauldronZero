#pragma once

#include "CauldronZero/Macros.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/EventRefObjSerializer.h>

class Actor;

namespace CauldronZero::event {

// Triggered before an actor rides another actor.
// This event can be cancelled to prevent the ride.
class ActorRideBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Actor& mVehicle;
    Actor& mPassenger;

public:
    constexpr explicit ActorRideBeforeEvent(Actor& vehicle, Actor& passenger)
    : mVehicle(vehicle),
      mPassenger(passenger) {}

    CZ_API Actor& getVehicle() const;
    CZ_API Actor& getPassenger() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// Triggered after an actor has ridden another actor.
class ActorRideAfterEvent final : public ll::event::Event {
protected:
    Actor& mVehicle;
    Actor& mPassenger;

public:
    constexpr explicit ActorRideAfterEvent(Actor& vehicle, Actor& passenger)
    : mVehicle(vehicle),
      mPassenger(passenger) {}

    CZ_API Actor& getVehicle() const;
    CZ_API Actor& getPassenger() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// Triggered after an actor has stopped riding another actor.
// This event cannot be cancelled.
class ActorStopRidingEvent final : public ll::event::Event {
protected:
    Actor& mVehicle;
    Actor& mPassenger;

public:
    constexpr explicit ActorStopRidingEvent(Actor& vehicle, Actor& passenger)
    : mVehicle(vehicle),
      mPassenger(passenger) {}

    CZ_API Actor& getVehicle() const;
    CZ_API Actor& getPassenger() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerActorRideEventHooks();
void unregisterActorRideEventHooks();

} // namespace CauldronZero::event
