#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/block/SculkSpreader.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class IBlockWorldGenAPI;
class Random;

namespace CauldronZero::event {

// 此事件在幽匿方块尝试生长之前触发。
// 它是可取消的，用以阻止该行为。
class SculkBlockGrowthBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    IBlockWorldGenAPI& mTarget;
    BlockSource* mRegion;
    BlockPos const& mPos;
    Random& mRandom;
    SculkSpreader& mSpreader;

public:
    constexpr explicit SculkBlockGrowthBeforeEvent(
        IBlockWorldGenAPI& target,
        BlockSource* region,
        BlockPos const& pos,
        Random& random,
        SculkSpreader& spreader
    )
    : mTarget(target),
      mRegion(region),
      mPos(pos),
      mRandom(random),
      mSpreader(spreader) {}

    CZ_API IBlockWorldGenAPI& getTarget() const;
    CZ_API BlockSource* getRegion() const;
    CZ_API BlockPos const& getPos() const;
    CZ_API Random& getRandom() const;
    CZ_API SculkSpreader& getSpreader() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在幽匿方块生长之后触发。
class SculkBlockGrowthAfterEvent final : public ll::event::Event {
protected:
    IBlockWorldGenAPI& mTarget;
    BlockSource* mRegion;
    BlockPos const& mPos;
    Random& mRandom;
    SculkSpreader& mSpreader;

public:
    constexpr explicit SculkBlockGrowthAfterEvent(
        IBlockWorldGenAPI& target,
        BlockSource* region,
        BlockPos const& pos,
        Random& random,
        SculkSpreader& spreader
    )
    : mTarget(target),
      mRegion(region),
      mPos(pos),
      mRandom(random),
      mSpreader(spreader) {}

    CZ_API IBlockWorldGenAPI& getTarget() const;
    CZ_API BlockSource* getRegion() const;
    CZ_API BlockPos const& getPos() const;
    CZ_API Random& getRandom() const;
    CZ_API SculkSpreader& getSpreader() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerSculkBlockGrowthEventHooks();
void unregisterSculkBlockGrowthEventHooks();

} // namespace CauldronZero::event
