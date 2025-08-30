#include "MobPickupItemEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/actor/Mob.h"
#include "mc/world/actor/ai/goal/PickupItemsGoal.h"
#include "mc/world/actor/item/ItemActor.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>


namespace CauldronZero::event {

// --- MobPickupItemBeforeEvent ---
Mob&           MobPickupItemBeforeEvent::getMob() const { return mMob; }
ItemActor&     MobPickupItemBeforeEvent::getItemActor() const { return mItemActor; }
PickupItemsGoal& MobPickupItemBeforeEvent::getGoal() const { return mGoal; } // 新增

void MobPickupItemBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["mob"]       = ll::event::serializeRefObj(mMob);
    nbt["itemActor"] = ll::event::serializeRefObj(mItemActor);
    nbt["goal"]      = ll::event::serializeRefObj(mGoal); // 新增
}

class MobPickupItemBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::MobPickupItemBeforeEvent> {};

// --- MobPickupItemAfterEvent ---
Mob&           MobPickupItemAfterEvent::getMob() const { return mMob; }
ItemActor&     MobPickupItemAfterEvent::getItemActor() const { return mItemActor; }
PickupItemsGoal& MobPickupItemAfterEvent::getGoal() const { return mGoal; } // 新增

void MobPickupItemAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["mob"]       = ll::event::serializeRefObj(mMob);
    nbt["itemActor"] = ll::event::serializeRefObj(mItemActor);
    nbt["goal"]      = ll::event::serializeRefObj(mGoal); // 新增
}

class MobPickupItemAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::MobPickupItemAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    MobPickupItemHook,
    HookPriority::Normal,
    PickupItemsGoal,
    &PickupItemsGoal::_pickItemUp,
    void,
    ::ItemActor* itemActor
) {
    try {


        if (itemActor) {
            auto beforeEvent = CauldronZero::event::MobPickupItemBeforeEvent(this->mMob, *itemActor, *this); // 修改
            ll::event::EventBus::getInstance().publish(beforeEvent);

            if (beforeEvent.isCancelled()) {
                return;
            }

            origin(itemActor);

            auto afterEvent = CauldronZero::event::MobPickupItemAfterEvent(this->mMob, *itemActor, *this); // 修改
            ll::event::EventBus::getInstance().publish(afterEvent);
        } else {
            origin(itemActor);
        }
    } catch (const SEH_Exception& e) {
        logger.warn(
            "MobPickupItemEventHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(itemActor);
    } catch (const std::exception& e) {
        logger.warn("MobPickupItemEventHook 发生 C++ 异常: {}", e.what());
        origin(itemActor);
    } catch (...) {
        logger.warn("MobPickupItemEventHook 发生未知异常！");
        origin(itemActor);
    }
}

void registerMobPickupItemEventHooks() { MobPickupItemHook::hook(); }

void unregisterMobPickupItemEventHooks() { MobPickupItemHook::unhook(); }

} // namespace CauldronZero::event
