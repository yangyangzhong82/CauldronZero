#pragma once

#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/actor/ai/goal/PickupItemsGoal.h" // 新增
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Mob;
class ItemActor;

namespace CauldronZero::event {

// 在生物尝试拾取物品之前触发。
// 它是可取消的，用以阻止该行为。
class MobPickupItemBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Mob&           mMob;
    ItemActor&     mItemActor;
    PickupItemsGoal& mGoal; // 新增

public:
    constexpr explicit MobPickupItemBeforeEvent(Mob& mob, ItemActor& itemActor, PickupItemsGoal& goal)
    : mMob(mob),
      mItemActor(itemActor),
      mGoal(goal) {} // 修改构造函数

    CZ_API Mob&           getMob() const;
    CZ_API ItemActor&     getItemActor() const;
    CZ_API PickupItemsGoal& getGoal() const; // 新增

    virtual void serialize(CompoundTag& nbt) const override;
};

// 在生物完成拾取物品之后触发。
class MobPickupItemAfterEvent final : public ll::event::Event {
protected:
    Mob&           mMob;
    ItemActor&     mItemActor;
    PickupItemsGoal& mGoal; // 新增

public:
    constexpr explicit MobPickupItemAfterEvent(Mob& mob, ItemActor& itemActor, PickupItemsGoal& goal)
    : mMob(mob),
      mItemActor(itemActor),
      mGoal(goal) {} // 修改构造函数

    CZ_API Mob&           getMob() const;
    CZ_API ItemActor&     getItemActor() const;
    CZ_API PickupItemsGoal& getGoal() const; // 新增

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerMobPickupItemEventHooks();
void unregisterMobPickupItemEventHooks();

} // namespace CauldronZero::event
