#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

// Forward declarations
class BlockSource;
class BlockPos;

namespace CauldronZero::event {

// 此事件在龙蛋传送之前触发。
// 它是可取消的。
class DragonEggTeleportBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource& mRegion;
    const BlockPos& mOldPos;
    BlockPos& mNewPos;

public:
    constexpr explicit DragonEggTeleportBeforeEvent(
        BlockSource&    region,
        const BlockPos& oldPos,
        BlockPos&       newPos
    )
    : mRegion(region),
      mOldPos(oldPos),
      mNewPos(newPos) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getOldPos() const;
    CZ_API BlockPos&       getNewPos() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在龙蛋传送之后触发。
// 此事件不可取消。
class DragonEggTeleportAfterEvent final : public ll::event::Event {
protected:
    BlockSource&    mRegion;
    const BlockPos& mOldPos;
    const BlockPos& mNewPos;

public:
    constexpr explicit DragonEggTeleportAfterEvent(
        BlockSource&    region,
        const BlockPos& oldPos,
        const BlockPos& newPos
    )
    : mRegion(region),
      mOldPos(oldPos),
      mNewPos(newPos) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getOldPos() const;
    CZ_API const BlockPos& getNewPos() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerDragonEggBlockEventHooks();
void unregisterDragonEggBlockEventHooks();

} // namespace CauldronZero::event
