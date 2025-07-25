#pragma once


#include "CauldronZero/Macros.h"
#include "mc/world/level/dimension/Dimension.h" 
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include "mc/nbt/CompoundTag.h" 
#include <ll/api/event/EventRefObjSerializer.h>
class Player;
namespace CauldronZero::event {


class PlayerChangeDimensionBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&       mPlayer;
    DimensionType mFromDimension;
    DimensionType mToDimension;

public:
    constexpr explicit PlayerChangeDimensionBeforeEvent(Player& player, DimensionType fromDim, DimensionType toDim)
    : mPlayer(player),
      mFromDimension(fromDim),
      mToDimension(toDim) {}

    CZ_API Player&       getPlayer() const;
    CZ_API DimensionType getFromDimension() const;
    CZ_API DimensionType getToDimension() const;

    virtual void serialize(CompoundTag& nbt) const override; // Declared serialize method
};

class PlayerChangeDimensionAfterEvent final : public ll::event::Event {
protected:
    Player&       mPlayer;
    DimensionType mFromDimension;
    DimensionType mToDimension;

public:
    constexpr explicit PlayerChangeDimensionAfterEvent(Player& player, DimensionType fromDim, DimensionType toDim)
    : mPlayer(player),
      mFromDimension(fromDim),
      mToDimension(toDim) {}

    CZ_API Player&       getPlayer() const;
    CZ_API DimensionType getFromDimension() const;
    CZ_API DimensionType getToDimension() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerPlayerChangeDimensionHooks();

void unregisterPlayerChangeDimensionHooks();
} // namespace CauldronZero::event
