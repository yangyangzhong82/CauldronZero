#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/BlockPos.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class PistonBlockActor;

namespace CauldronZero::event {

// 此事件在活塞推动方块之前触发。
// 它是可取消的，用以阻止该行为。
class PistonPushBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    PistonBlockActor& mPiston;
    BlockSource& mRegion;
    BlockPos const& mCurPos;
    uchar mCurBranchFacing;
    uchar mPistonMoveFacing;

public:
    constexpr explicit PistonPushBeforeEvent(
        PistonBlockActor& piston,
        BlockSource& region,
        BlockPos const& curPos,
        uchar curBranchFacing,
        uchar pistonMoveFacing
    )
    : mPiston(piston),
      mRegion(region),
      mCurPos(curPos),
      mCurBranchFacing(curBranchFacing),
      mPistonMoveFacing(pistonMoveFacing) {}

    CZ_API PistonBlockActor& getPiston() const;
    CZ_API BlockSource& getRegion() const;
    CZ_API BlockPos const& getCurPos() const;
    CZ_API uchar getCurBranchFacing() const;
    CZ_API uchar getPistonMoveFacing() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在活塞推动方块之后触发。
class PistonPushAfterEvent final : public ll::event::Event {
protected:
    PistonBlockActor& mPiston;
    BlockSource& mRegion;
    BlockPos const& mCurPos;
    uchar mCurBranchFacing;
    uchar mPistonMoveFacing;
    bool mResult;

public:
    constexpr explicit PistonPushAfterEvent(
        PistonBlockActor& piston,
        BlockSource& region,
        BlockPos const& curPos,
        uchar curBranchFacing,
        uchar pistonMoveFacing,
        bool result
    )
    : mPiston(piston),
      mRegion(region),
      mCurPos(curPos),
      mCurBranchFacing(curBranchFacing),
      mPistonMoveFacing(pistonMoveFacing),
      mResult(result) {}

    CZ_API PistonBlockActor& getPiston() const;
    CZ_API BlockSource& getRegion() const;
    CZ_API BlockPos const& getCurPos() const;
    CZ_API uchar getCurBranchFacing() const;
    CZ_API uchar getPistonMoveFacing() const;
    CZ_API bool getResult() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerPistonPushEventHooks();
void unregisterPistonPushEventHooks();

} // namespace CauldronZero::event
