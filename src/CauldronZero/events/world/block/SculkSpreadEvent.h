#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/BlockPos.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include <optional>
#include <utility> // For std::pair


class MultifaceSpreader;
class Block;
class IBlockWorldGenAPI;
class IBlockSource;
class BlockPos;


namespace CauldronZero::event {

// 此事件在 MultifaceSpreader 尝试从一个面朝向某个方向传播之前触发。
// 它是可取消的，用以阻止该行为。
class SculkSpreadBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    MultifaceSpreader& mSpreader;
    IBlockWorldGenAPI& mTarget;
    Block const&       mSelfBlock;
    Block const&       mBlock;
    BlockPos const&    mPos;
    uchar              mStartingFace;
    uchar              mSpreadDirection;

public:
    constexpr explicit SculkSpreadBeforeEvent(
        MultifaceSpreader& spreader,
        IBlockWorldGenAPI& target,
        Block const&       selfBlock,
        Block const&       block,
        BlockPos const&    pos,
        uchar              startingFace,
        uchar              spreadDirection
    )
    : mSpreader(spreader),
      mTarget(target),
      mSelfBlock(selfBlock),
      mBlock(block),
      mPos(pos),
      mStartingFace(startingFace),
      mSpreadDirection(spreadDirection) {}

    CZ_API MultifaceSpreader& getSpreader() const;
    CZ_API IBlockWorldGenAPI& getTarget() const;
    CZ_API Block const&       getSelfBlock() const;
    CZ_API Block const&       getBlock() const;
    CZ_API BlockPos const&    getPos() const;
    CZ_API uchar              getStartingFace() const;
    CZ_API uchar              getSpreadDirection() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在 MultifaceSpreader 完成从一个面朝向某个方向传播之后触发。
class SculkSpreadAfterEvent final : public ll::event::Event {
protected:
    MultifaceSpreader&                                    mSpreader;
    IBlockWorldGenAPI&                                    mTarget;
    Block const&                                          mSelfBlock;
    Block const&                                          mBlock;
    BlockPos const&                                       mPos;
    uchar                                                 mStartingFace;
    uchar                                                 mSpreadDirection;
    std::optional<std::pair<BlockPos const, uchar const>> mResult;

public:
    constexpr explicit SculkSpreadAfterEvent(
        MultifaceSpreader&                                    spreader,
        IBlockWorldGenAPI&                                    target,
        Block const&                                          selfBlock,
        Block const&                                          block,
        BlockPos const&                                       pos,
        uchar                                                 startingFace,
        uchar                                                 spreadDirection,
        std::optional<std::pair<BlockPos const, uchar const>> result
    )
    : mSpreader(spreader),
      mTarget(target),
      mSelfBlock(selfBlock),
      mBlock(block),
      mPos(pos),
      mStartingFace(startingFace),
      mSpreadDirection(spreadDirection),
      mResult(result) {}

    CZ_API MultifaceSpreader& getSpreader() const;
    CZ_API IBlockWorldGenAPI& getTarget() const;
    CZ_API Block const&       getSelfBlock() const;
    CZ_API Block const&       getBlock() const;
    CZ_API BlockPos const&    getPos() const;
    CZ_API uchar              getStartingFace() const;
    CZ_API uchar              getSpreadDirection() const;
    CZ_API std::optional<std::pair<BlockPos const, uchar const>> getResult() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerSculkSpreadEventHooks();
void unregisterSculkSpreadEventHooks();

} // namespace CauldronZero::event
