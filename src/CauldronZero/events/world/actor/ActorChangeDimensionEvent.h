#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/dimension/Dimension.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>


class Actor;
 
namespace CauldronZero::event {

    // 此事件在 Actor 尝试跨维度传送之前触发。
    // 它是可取消的，用以阻止该行为。
    class ActorChangeDimensionBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
    protected:
        Actor&        mActor;
        DimensionType mFromDimension;
        DimensionType mToDimension;

    public:
        constexpr explicit ActorChangeDimensionBeforeEvent(Actor& actor, DimensionType fromDim, DimensionType toDim)
        : mActor(actor),
          mFromDimension(fromDim),
          mToDimension(toDim) {}

        CZ_API Actor&        getActor() const;
        CZ_API DimensionType getFromDimension() const;
        CZ_API DimensionType getToDimension() const;

        virtual void serialize(CompoundTag& nbt) const override; // Declared serialize method
    };

    // 此事件在 Actor 完成跨维度传送之后触发。
    class ActorChangeDimensionAfterEvent final : public ll::event::Event {
    protected:
        Actor&        mActor;
        DimensionType mFromDimension;
        DimensionType mToDimension;

    public:
        constexpr explicit ActorChangeDimensionAfterEvent(Actor& actor, DimensionType fromDim, DimensionType toDim)
        : mActor(actor),
          mFromDimension(fromDim),
          mToDimension(toDim) {}

        CZ_API Actor&        getActor() const;
        CZ_API DimensionType getFromDimension() const;
        CZ_API DimensionType getToDimension() const;

        virtual void serialize(CompoundTag& nbt) const override;
    };

    void registerActorChangeDimensionEventHooks();

    void unregisterActorChangeDimensionEventHooks();
} // namespace CauldronZero::event
