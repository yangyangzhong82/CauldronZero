#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/BlockPos.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class ServerPlayer;

namespace CauldronZero::event {

class PlayerAttackBlockBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    ServerPlayer& mPlayer;
    const BlockPos& mBlockPos;
    int mFace;

public:
    constexpr explicit PlayerAttackBlockBeforeEvent(ServerPlayer& player, const BlockPos& pos, int face)
    : mPlayer(player),
      mBlockPos(pos),
      mFace(face) {}

    CZ_API ServerPlayer& getPlayer() const;
    CZ_API const BlockPos& getBlockPos() const;
    CZ_API int getFace() const;

    void serialize(CompoundTag& nbt) const override;
};

class PlayerAttackBlockAfterEvent final : public ll::event::Event {
protected:
    ServerPlayer& mPlayer;
    const BlockPos& mBlockPos;
    int mFace;

public:
    constexpr explicit PlayerAttackBlockAfterEvent(ServerPlayer& player, const BlockPos& pos, int face)
    : mPlayer(player),
      mBlockPos(pos),
      mFace(face) {}

    CZ_API ServerPlayer& getPlayer() const;
    CZ_API const BlockPos& getBlockPos() const;
    CZ_API int getFace() const;

    void serialize(CompoundTag& nbt) const override;
};

void registerPlayerAttackBlockHooks();
void unregisterPlayerAttackBlockHooks();

} // namespace CauldronZero::event
