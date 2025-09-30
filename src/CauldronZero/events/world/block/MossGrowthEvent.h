#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <vector>

// Forward declarations
class IBlockWorldGenAPI;
class Random;
#include "mc/world/level/BlockPos.h"

namespace CauldronZero::event {

// 此事件在苔藓生长之前触发。
// 它是可取消的。
class MossGrowthBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    IBlockWorldGenAPI& mTarget;
    Random&            mRandom;
    const BlockPos&    mOrigin;
    int                mXRadius;
    int                mZRadius;

public:
    constexpr explicit MossGrowthBeforeEvent(
        IBlockWorldGenAPI& target,
        Random&            random,
        const BlockPos&    origin,
        int                xRadius,
        int                zRadius
    )
    : mTarget(target),
      mRandom(random),
      mOrigin(origin),
      mXRadius(xRadius),
      mZRadius(zRadius) {}

    CZ_API IBlockWorldGenAPI& getTarget() const;
    CZ_API Random&            getRandom() const;
    CZ_API const BlockPos&    getOrigin() const;
    CZ_API int                getXRadius() const;
    CZ_API int                getZRadius() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在苔藓生长之后触发。
// 此事件不可取消。
class MossGrowthAfterEvent final : public ll::event::Event {
protected:
    IBlockWorldGenAPI& mTarget;
    Random&            mRandom;
    const BlockPos&    mOrigin;
    int                mXRadius;
    int                mZRadius;
    std::vector<BlockPos> mResult;

public:
    constexpr explicit MossGrowthAfterEvent(
        IBlockWorldGenAPI&    target,
        Random&               random,
        const BlockPos&       origin,
        int                   xRadius,
        int                   zRadius,
        std::vector<BlockPos> result
    )
    : mTarget(target),
      mRandom(random),
      mOrigin(origin),
      mXRadius(xRadius),
      mZRadius(zRadius),
      mResult(std::move(result)) {}

    CZ_API IBlockWorldGenAPI&    getTarget() const;
    CZ_API Random&               getRandom() const;
    CZ_API const BlockPos&       getOrigin() const;
    CZ_API int                   getXRadius() const;
    CZ_API int                   getZRadius() const;
    CZ_API std::vector<BlockPos> getResult() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerMossGrowthEventHooks();
void unregisterMossGrowthEventHooks();

} // namespace CauldronZero::event
