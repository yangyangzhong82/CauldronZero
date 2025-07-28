#pragma once

#include "CauldronZero/Macros.h"
#include "mc/world/actor/boss/WitherBoss.h"
#include "mc/world/phys/AABB.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

class Level;
class BlockSource;
class CompoundTag;

namespace CauldronZero::event {

class WitherDestroyBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    WitherBoss&                  mWither;
    Level&                       mLevel;
    AABB const&                  mAABB;
    BlockSource&                 mRegion;
    int&                         mRange;
    WitherBoss::WitherAttackType mAttackType;

public:
    constexpr explicit WitherDestroyBeforeEvent(
        WitherBoss& wither,
        Level&                       level,
        AABB const&                  aabb,
        BlockSource&                 region,
        int&                         range,
        WitherBoss::WitherAttackType attackType
    )
    : mWither(wither),
      mLevel(level),
      mAABB(aabb),
      mRegion(region),
      mRange(range),
      mAttackType(attackType) {}

    CZ_API WitherBoss&                  getWither() const;
    CZ_API Level&                       getLevel() const;
    CZ_API AABB const&                  getAABB() const;
    CZ_API BlockSource&                 getRegion() const;
    CZ_API int&                         getRange() const;
    CZ_API WitherBoss::WitherAttackType getAttackType() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

class WitherDestroyAfterEvent final : public ll::event::Event {
protected:
    WitherBoss&                  mWither;
    Level&                       mLevel;
    AABB const&                  mAABB;
    BlockSource&                 mRegion;
    int                          mRange;
    WitherBoss::WitherAttackType mAttackType;

public:
    constexpr explicit WitherDestroyAfterEvent(
        WitherBoss&                  wither,
        Level&                       level,
        AABB const&                  aabb,
        BlockSource&                 region,
        int                          range,
        WitherBoss::WitherAttackType attackType
    )
    : mWither(wither),
      mLevel(level),
      mAABB(aabb),
      mRegion(region),
      mRange(range),
      mAttackType(attackType) {}

    CZ_API WitherBoss&                  getWither() const;
    CZ_API Level&                       getLevel() const;
    CZ_API AABB const&                  getAABB() const;
    CZ_API BlockSource&                 getRegion() const;
    CZ_API int                          getRange() const;
    CZ_API WitherBoss::WitherAttackType getAttackType() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerWitherDestroyEventHooks();
void unregisterWitherDestroyEventHooks();

} // namespace CauldronZero::event
