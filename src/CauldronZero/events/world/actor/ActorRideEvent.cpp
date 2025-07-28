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

void registerActorRideEventHooks() { ActorRideEventHook::hook(); }
void unregisterActorRideEventHooks() { ActorRideEventHook::unhook(); }

} // namespace CauldronZero::event
