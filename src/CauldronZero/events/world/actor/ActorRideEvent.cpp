#include "ActorRideEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/Actor.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include <ll/api/event/EventRefObjSerializer.h>

namespace CauldronZero::event {

// --- ActorRideBeforeEvent ---
Actor& ActorRideBeforeEvent::getVehicle() const { return mVehicle; }
Actor& ActorRideBeforeEvent::getPassenger() const { return mPassenger; }

void ActorRideBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["vehicle"]   = ll::event::serializeRefObj(mVehicle);
    nbt["passenger"] = ll::event::serializeRefObj(mPassenger);
}

class ActorRideBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorRideBeforeEvent> {};

// --- ActorRideAfterEvent ---
Actor& ActorRideAfterEvent::getVehicle() const { return mVehicle; }
Actor& ActorRideAfterEvent::getPassenger() const { return mPassenger; }

void ActorRideAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["vehicle"]   = ll::event::serializeRefObj(mVehicle);
    nbt["passenger"] = ll::event::serializeRefObj(mPassenger);
}

class ActorRideAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorRideAfterEvent> {};

// --- ActorStopRidingEvent ---
Actor& ActorStopRidingEvent::getVehicle() const { return mVehicle; }
Actor& ActorStopRidingEvent::getPassenger() const { return mPassenger; }

void ActorStopRidingEvent::serialize(CompoundTag& nbt) const {
    nbt["vehicle"]   = ll::event::serializeRefObj(mVehicle);
    nbt["passenger"] = ll::event::serializeRefObj(mPassenger);
}

class ActorStopRidingEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorStopRidingEvent> {};


LL_TYPE_INSTANCE_HOOK(
    ActorRideEventHook,
    ll::memory::HookPriority::Normal,
    Actor,
    &Actor::$addPassenger,
    void,
    Actor& passenger
) {
    try {
        auto& vehicle = *this;
        auto  event   = ActorRideBeforeEvent(vehicle, passenger);
        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return;
        }

        origin(passenger);

        auto afterEvent = ActorRideAfterEvent(vehicle, passenger);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "ActorRideEventHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(passenger);
    } catch (const std::exception& e) {
        logger.warn("ActorRideEventHook 发生 C++ 异常: {}", e.what());
        origin(passenger);
    } catch (...) {
        logger.warn("ActorRideEventHook 发生未知异常！");
        origin(passenger);
    }
}

LL_TYPE_INSTANCE_HOOK(
    ActorRideEventHook2,
    ll::memory::HookPriority::Normal,
    Actor,
    &Actor::stopRiding,
    void,
    bool exitFromPassenger,
    bool actorIsBeingDestroyed,
    bool switchingVehicles,
    bool isBeingTeleported
) {
    try {
        auto& passenger = *this; // *this 是正在停止乘骑的 Actor，也就是乘客
        Actor* vehicle = passenger.getVehicle(); // 获取乘客的载具

        origin(exitFromPassenger, actorIsBeingDestroyed, switchingVehicles, isBeingTeleported);

        if (vehicle) {
            auto stopRidingEvent = ActorStopRidingEvent(*vehicle, passenger);
            ll::event::EventBus::getInstance().publish(stopRidingEvent);
        }
    } catch (const SEH_Exception& e) {
        logger.warn(
            "ActorRideEventHook2 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(exitFromPassenger, actorIsBeingDestroyed, switchingVehicles, isBeingTeleported);
    } catch (const std::exception& e) {
        logger.warn("ActorRideEventHook2 发生 C++ 异常: {}", e.what());
        origin(exitFromPassenger, actorIsBeingDestroyed, switchingVehicles, isBeingTeleported);
    } catch (...) {
        logger.warn("ActorRideEventHook2 发生未知异常！");
        origin(exitFromPassenger, actorIsBeingDestroyed, switchingVehicles, isBeingTeleported);
    }
}

void registerActorRideEventHooks() {
    ActorRideEventHook::hook();
    ActorRideEventHook2::hook();
}
void unregisterActorRideEventHooks() {
    ActorRideEventHook::unhook();
    ActorRideEventHook2::unhook();
}

} // namespace CauldronZero::event
