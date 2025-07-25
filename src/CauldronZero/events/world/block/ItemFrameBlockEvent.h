#pragma once

#include "CauldronZero/Macros.h"
#include "mc/world/level/block/block_events/BlockPlayerInteractEvent.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

class Player;
class BlockPos;
class BlockSource;

namespace CauldronZero::event {

class ItemFrameBlockAttackBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&         mPlayer;
    BlockPos const& mPos;

public:
    constexpr explicit ItemFrameBlockAttackBeforeEvent(Player& player, BlockPos const& pos)
    : mPlayer(player),
      mPos(pos) {}

    CZ_API Player&         getPlayer() const;
    CZ_API BlockPos const& getPos() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

class ItemFrameBlockAttackAfterEvent final : public ll::event::Event {
protected:
    Player&         mPlayer;
    BlockPos const& mPos;

public:
    constexpr explicit ItemFrameBlockAttackAfterEvent(Player& player, BlockPos const& pos)
    : mPlayer(player),
      mPos(pos) {}

    CZ_API Player&         getPlayer() const;
    CZ_API BlockPos const& getPos() const;

    virtual void serialize(CompoundTag& nbt) const override;
};


class ItemFrameBlockUseBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockEvents::BlockPlayerInteractEvent& mEventData;

public:
    constexpr explicit ItemFrameBlockUseBeforeEvent(BlockEvents::BlockPlayerInteractEvent& eventData)
    : mEventData(eventData) {}

    CZ_API Player&           getPlayer() const;
    CZ_API Vec3              getPos() const;
    CZ_API BlockSource const& getBlockSource() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

class ItemFrameBlockUseAfterEvent final : public ll::event::Event {
protected:
    BlockEvents::BlockPlayerInteractEvent& mEventData;

public:
    constexpr explicit ItemFrameBlockUseAfterEvent(BlockEvents::BlockPlayerInteractEvent& eventData)
    : mEventData(eventData) {}

    CZ_API Player&           getPlayer() const;
    CZ_API Vec3              getPos() const;
    CZ_API BlockSource const& getBlockSource() const;

    virtual void serialize(CompoundTag& nbt) const override;
};
void registerFrameBlockuseEventHooks();
void unregisterFrameBlockuseEventHooks();
void registerFrameBlockattackEventHooks();
void unregisterFrameBlockattackEventHooks();
} // namespace CauldronZero::event
