#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

// Forward declarations
class BlockSource;
class BlockPos;

namespace CauldronZero::event {

// 此事件在液体方块尝试扩散之前触发。
// 它是可取消的。
class LiquidBlockTrySpreadBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    int             mNeighbor;
    const BlockPos& mFlowFromPos;
    unsigned char   mFlowFromDirection;

public:
    constexpr explicit LiquidBlockTrySpreadBeforeEvent(
        BlockSource&    region,
        const BlockPos& pos,
        int             neighbor,
        const BlockPos& flowFromPos,
        unsigned char   flowFromDirection
    )
    : mRegion(region),
      mPos(pos),
      mNeighbor(neighbor),
      mFlowFromPos(flowFromPos),
      mFlowFromDirection(flowFromDirection) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API int             getNeighbor() const;
    CZ_API const BlockPos& getFlowFromPos() const;
    CZ_API unsigned char   getFlowFromDirection() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在液体方块尝试扩散之后触发。
// 此事件不可取消。
class LiquidBlockTrySpreadAfterEvent final : public ll::event::Event {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    int             mNeighbor;
    const BlockPos& mFlowFromPos;
    unsigned char   mFlowFromDirection;

public:
    constexpr explicit LiquidBlockTrySpreadAfterEvent(
        BlockSource&    region,
        const BlockPos& pos,
        int             neighbor,
        const BlockPos& flowFromPos,
        unsigned char   flowFromDirection
    )
    : mRegion(region),
      mPos(pos),
      mNeighbor(neighbor),
      mFlowFromPos(flowFromPos),
      mFlowFromDirection(flowFromDirection) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API int             getNeighbor() const;
    CZ_API const BlockPos& getFlowFromPos() const;
    CZ_API unsigned char   getFlowFromDirection() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerLiquidTrySpreadEventHooks();
void unregisterLiquidTrySpreadEventHooks();

} // namespace CauldronZero::event
