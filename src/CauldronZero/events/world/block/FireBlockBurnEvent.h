#pragma once
#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

// Forward declarations
class BlockSource;
class Block;
class BlockPos;
namespace CauldronZero::event {

// 此事件在火焰试图烧毁一个方块之前触发。
// 它是可取消的，以阻止方块被烧毁。
class FireBlockBurnBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource&    mRegion;      // 事件发生的区域。
    const BlockPos& mFirePos;     // 火焰方块的位置。
    const BlockPos& mBurnedPos;   // 即将被烧毁的方块的位置。
    const Block&    mBurnedBlock; // 即将被烧毁的方块实例。

public:
    constexpr explicit FireBlockBurnBeforeEvent(
        BlockSource&    region,
        const BlockPos& firePos,
        const BlockPos& burnedPos,
        const Block&    burnedBlock
    )
    : mRegion(region),
      mFirePos(firePos),
      mBurnedPos(burnedPos),
      mBurnedBlock(burnedBlock) {}

    // 获取事件发生的ource。
    CZ_API BlockSource& getRegion() const;

    // 获取火焰方块的位置。
    CZ_API const BlockPos& getFirePos() const;

    // 获取即将被烧毁的方块的位置。
    CZ_API const BlockPos& getBurnedPos() const;

    // 获取即将被烧毁的方块实例。
    CZ_API const Block& getBurnedBlock() const;

    virtual void serialize(CompoundTag& nbt) const override;
};


// 此事件在火焰成功烧毁一个方块之后触发。
// 此事件不可取消。
class FireBlockBurnAfterEvent final : public ll::event::Event {
protected:
    BlockSource&    mRegion;    // 事件发生的区域。
    const BlockPos& mFirePos;   // 火焰方块的位置。
    const BlockPos& mBurnedPos; // 被烧毁的方块的位置。

public:
    constexpr explicit FireBlockBurnAfterEvent(BlockSource& region, const BlockPos& firePos, const BlockPos& burnedPos)
    : mRegion(region),
      mFirePos(firePos),
      mBurnedPos(burnedPos) {}

    // 获取事件发生的区域。
    CZ_API BlockSource& getRegion() const;

    // 获取火焰方块的位置。
    CZ_API const BlockPos& getFirePos() const;

    // 获取被烧毁的方块的位置。
    CZ_API const BlockPos& getBurnedPos() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

} // namespace CauldronZero::event
