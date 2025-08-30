#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

// Forward declarations
class BlockSource;
class BlockPos;
class Randomize;

namespace CauldronZero::event {

// 此事件在火焰尝试烧毁方块之前触发。
// 它是可取消的。
class FireBlockBurnBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    int             mChance;
    Randomize&      mRandomize;
    int             mAge;

public:
    constexpr explicit FireBlockBurnBeforeEvent(
        BlockSource&    region,
        const BlockPos& pos,
        int             chance,
        Randomize&      randomize,
        int             age
    )
    : mRegion(region),
      mPos(pos),
      mChance(chance),
      mRandomize(randomize),
      mAge(age) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API int             getChance() const;
    CZ_API Randomize&      getRandomize() const;
    CZ_API int             getAge() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在火焰成功烧毁方块之后触发。
// 此事件不可取消。
class FireBlockBurnAfterEvent final : public ll::event::Event {
protected:
    BlockSource&    mRegion;
    const BlockPos& mPos;
    int             mChance;
    Randomize&      mRandomize;
    int             mAge;

public:
    constexpr explicit FireBlockBurnAfterEvent(
        BlockSource&    region,
        const BlockPos& pos,
        int             chance,
        Randomize&      randomize,
        int             age
    )
    : mRegion(region),
      mPos(pos),
      mChance(chance),
      mRandomize(randomize),
      mAge(age) {}

    CZ_API BlockSource&    getRegion() const;
    CZ_API const BlockPos& getPos() const;
    CZ_API int             getChance() const;
    CZ_API Randomize&      getRandomize() const;
    CZ_API int             getAge() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerFireBlockEventHooks();
void unregisterFireBlockEventHooks();
} // namespace CauldronZero::event
