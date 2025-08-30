#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/item/ItemStack.h"
#include "mc/deps/core/math/Vec3.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Actor;
class BlockSource;
class ItemActor;

namespace CauldronZero::event {

// 此事件在掉落物生成之前触发。
// 它是可取消的，用以阻止该行为。
class SpawnItemActorBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource& mRegion;
    ItemStack const& mInst;
    Actor* mSpawner;
    Vec3 const& mPos;
    int mThrowTime;

public:
    constexpr explicit SpawnItemActorBeforeEvent(
        BlockSource& region,
        ItemStack const& inst,
        Actor* spawner,
        Vec3 const& pos,
        int throwTime
    )
    : mRegion(region),
      mInst(inst),
      mSpawner(spawner),
      mPos(pos),
      mThrowTime(throwTime) {}

    CZ_API BlockSource& getRegion() const;
    CZ_API ItemStack const& getInst() const;
    CZ_API Actor* getSpawner() const;
    CZ_API Vec3 const& getPos() const;
    CZ_API int getThrowTime() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在掉落物生成之后触发。
class SpawnItemActorAfterEvent final : public ll::event::Event {
protected:
    BlockSource& mRegion;
    ItemStack const& mInst;
    Actor* mSpawner;
    Vec3 const& mPos;
    int mThrowTime;
    ItemActor* mItemActor;

public:
    constexpr explicit SpawnItemActorAfterEvent(
        BlockSource& region,
        ItemStack const& inst,
        Actor* spawner,
        Vec3 const& pos,
        int throwTime,
        ItemActor* itemActor
    )
    : mRegion(region),
      mInst(inst),
      mSpawner(spawner),
      mPos(pos),
      mThrowTime(throwTime),
      mItemActor(itemActor) {}

    CZ_API BlockSource& getRegion() const;
    CZ_API ItemStack const& getInst() const;
    CZ_API Actor* getSpawner() const;
    CZ_API Vec3 const& getPos() const;
    CZ_API int getThrowTime() const;
    CZ_API ItemActor* getItemActor() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerSpawnItemActorEventHooks();
void unregisterSpawnItemActorEventHooks();

} // namespace CauldronZero::event
