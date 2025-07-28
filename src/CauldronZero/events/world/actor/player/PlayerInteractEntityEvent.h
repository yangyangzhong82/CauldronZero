#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Player;
class Actor;
class Vec3;

namespace CauldronZero::event {

class PlayerInteractEntityBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&   mPlayer;
    Actor&    mEntity;
    Vec3 const& mInteractLocation;

public:
    explicit PlayerInteractEntityBeforeEvent(Player& player, Actor& entity, Vec3 const& interactLocation)
    : mPlayer(player),
      mEntity(entity),
      mInteractLocation(interactLocation) {}

    CZ_API Player& getPlayer() const;
    CZ_API Actor& getEntity() const;
    CZ_API Vec3 const& getInteractLocation() const;

    void serialize(CompoundTag& nbt) const override;
};

class PlayerInteractEntityAfterEvent final : public ll::event::Event {
protected:
    Player&   mPlayer;
    Actor&    mEntity;
    Vec3 const& mInteractLocation;

public:
    explicit PlayerInteractEntityAfterEvent(Player& player, Actor& entity, Vec3 const& interactLocation)
    : mPlayer(player),
      mEntity(entity),
      mInteractLocation(interactLocation) {}

    CZ_API Player& getPlayer() const;
    CZ_API Actor& getEntity() const;
    CZ_API Vec3 const& getInteractLocation() const;

    void serialize(CompoundTag& nbt) const override;
};

void registerPlayerInteractEntityHooks();
void unregisterPlayerInteractEntityHooks();

} // namespace CauldronZero::event
