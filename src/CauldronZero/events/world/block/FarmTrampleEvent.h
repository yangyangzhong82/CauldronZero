#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

// Forward declarations
class BlockSource;
class BlockPos;
class Actor;

namespace CauldronZero::event {

// 此事件在耕地被踩踏之前触发。
// 它是可取消的。
class FarmTrampleBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    Actor*          mActor;
    float           mFallDistance;

public:
    constexpr explicit FarmTrampleBeforeEvent(
        BlockSource&    region,
        const BlockPos& pos,
        Actor*          actor,
        float           fallDistance
    )
    : mRegion(region),
      mPos(pos),
      mActor(actor),
      mFallDistance(fallDistance) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API Actor*          getActor() const;
    CZ_API float           getFallDistance() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在耕地被踩踏之后触发。
// 此事件不可取消。
class FarmTrampleAfterEvent final : public ll::event::Event {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    Actor*          mActor;
    float           mFallDistance;

public:
    constexpr explicit FarmTrampleAfterEvent(
        BlockSource&    region,
        const BlockPos& pos,
        Actor*          actor,
        float           fallDistance
    )
    : mRegion(region),
      mPos(pos),
      mActor(actor),
      mFallDistance(fallDistance) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API Actor*          getActor() const;
    CZ_API float           getFallDistance() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerFarmTrampleEventHooks();
void unregisterFarmTrampleEventHooks();

} // namespace CauldronZero::event
