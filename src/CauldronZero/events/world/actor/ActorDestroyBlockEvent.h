#pragma once

#include "CauldronZero/Macros.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

class Actor;
class Block;
class Vec3;

namespace CauldronZero::event {

class ActorDestroyBlockBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Actor&       mActor;
    Block const& mBlock;
    Vec3 const&  mPos;

public:
    constexpr explicit ActorDestroyBlockBeforeEvent(Actor& actor, Block const& block, Vec3 const& pos)
    : mActor(actor),
      mBlock(block),
      mPos(pos) {}

    CZ_API Actor&       getActor() const;
    CZ_API Block const& getBlock() const;
    CZ_API Vec3 const&  getPos() const;
};
class ActorDestroyBlockAfterEvent final : public ll::event::Event {
protected:
    Actor&       mActor;
    Block const& mBlock;
    Vec3 const&  mPos;

public:
    constexpr explicit ActorDestroyBlockAfterEvent(Actor& actor, Block const& block, Vec3 const& pos)
    : mActor(actor),
      mBlock(block),
      mPos(pos) {}

    CZ_API Actor&      getActor() const;
    CZ_API Block const& getBlock() const;
    CZ_API Vec3 const&  getPos() const;
};
void registerActorDestroyBlockHooks();

void unregisterActorDestroyBlockHooks();
} // namespace CauldronZero::event
