#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

// Forward declarations
class BlockSource;
class BlockPos;

namespace CauldronZero::event {

// 此事件在液体方块扩散之前触发。
// 它是可取消的。
class LiquidSpreadBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    int             mDepth;
    bool            mPreserveExisting;

public:
    constexpr explicit LiquidSpreadBeforeEvent(
        BlockSource&    region,
        const BlockPos& pos,
        int             depth,
        bool            preserveExisting
    )
    : mRegion(region),
      mPos(pos),
      mDepth(depth),
      mPreserveExisting(preserveExisting) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API int             getDepth() const;
    CZ_API bool            getPreserveExisting() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在液体方块扩散之后触发。
// 此事件不可取消。
class LiquidSpreadAfterEvent final : public ll::event::Event {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    int             mDepth;
    bool            mPreserveExisting;

public:
    constexpr explicit LiquidSpreadAfterEvent(
        BlockSource&    region,
        const BlockPos& pos,
        int             depth,
        bool            preserveExisting
    )
    : mRegion(region),
      mPos(pos),
      mDepth(depth),
      mPreserveExisting(preserveExisting) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API int             getDepth() const;
    CZ_API bool            getPreserveExisting() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerLiquidSpreadEventHooks();
void unregisterLiquidSpreadEventHooks();

} // namespace CauldronZero::event
