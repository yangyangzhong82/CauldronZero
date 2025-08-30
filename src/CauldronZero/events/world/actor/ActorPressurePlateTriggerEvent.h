#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Actor;
class Block;
class BlockPos;

namespace CauldronZero::event {

    // 此事件在实体尝试触发压力板之前触发。
    // 它是可取消的，用以阻止该行为。
    class ActorPressurePlateTriggerBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
    protected:
        Actor&        mActor;
        Block const&  mBlock;
        BlockPos const& mPos;

    public:
        constexpr explicit ActorPressurePlateTriggerBeforeEvent(Actor& actor, Block const& block, BlockPos const& pos)
        : mActor(actor),
          mBlock(block),
          mPos(pos) {}

        CZ_API Actor&        getActor() const;
        CZ_API Block const&  getBlock() const;
        CZ_API BlockPos const& getPos() const;

        virtual void serialize(CompoundTag& nbt) const override;
    };

    // 此事件在实体完成触发压力板之后触发。
    class ActorPressurePlateTriggerAfterEvent final : public ll::event::Event {
    protected:
        Actor&        mActor;
        Block const&  mBlock;
        BlockPos const& mPos;

    public:
        constexpr explicit ActorPressurePlateTriggerAfterEvent(Actor& actor, Block const& block, BlockPos const& pos)
        : mActor(actor),
          mBlock(block),
          mPos(pos) {}

        CZ_API Actor&        getActor() const;
        CZ_API Block const&  getBlock() const;
        CZ_API BlockPos const& getPos() const;

        virtual void serialize(CompoundTag& nbt) const override;
    };

    void registerActorPressurePlateTriggerEventHooks();
    void unregisterActorPressurePlateTriggerEventHooks();
} // namespace CauldronZero::event
