#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

// Forward declarations
class BlockSource;
class BlockPos;
class Block;
class FallingBlockActor;

namespace CauldronZero::event {

// 此事件在方块开始下落之前触发。
// 它是可取消的。
class BlockFallBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    const Block&    mOldBlock;
    bool            mCreative;

public:
    constexpr explicit BlockFallBeforeEvent(
        BlockSource&    region,
        const BlockPos& pos,
        const Block&    oldBlock,
        bool            creative
    )
    : mRegion(region),
      mPos(pos),
      mOldBlock(oldBlock),
      mCreative(creative) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API const Block&    getOldBlock() const;
    CZ_API bool            isCreative() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在方块开始下落之后触发。
// 此事件不可取消。
class BlockFallAfterEvent final : public ll::event::Event {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    const Block&    mOldBlock;
    bool            mCreative;

public:
    constexpr explicit BlockFallAfterEvent(
        BlockSource&    region,
        const BlockPos& pos,
        const Block&    oldBlock,
        bool            creative
    )
    : mRegion(region),
      mPos(pos),
      mOldBlock(oldBlock),
      mCreative(creative) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API const Block&    getOldBlock() const;
    CZ_API bool            isCreative() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerBlockFallEventHooks();
void unregisterBlockFallEventHooks();

} // namespace CauldronZero::event
