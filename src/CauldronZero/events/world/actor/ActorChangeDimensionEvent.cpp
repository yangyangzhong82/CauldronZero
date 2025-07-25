

#include "ActorChangeDimensionEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/level/ActorDimensionTransferManager.h"
#include "mc/world/level/Level.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include "mc/nbt/CompoundTag.h" 
#include "mc/nbt/CompoundTagVariant.h"
#include <ll/api/event/EventRefObjSerializer.h> 


namespace CauldronZero::event {

// --- ActorChangeDimensionBeforeEvent ---
Actor&        ActorChangeDimensionBeforeEvent::getActor() const { return mActor; }
DimensionType ActorChangeDimensionBeforeEvent::getFromDimension() const { return mFromDimension; }
DimensionType ActorChangeDimensionBeforeEvent::getToDimension() const { return mToDimension; }

void ActorChangeDimensionBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["actor"]        = ll::event::serializeRefObj(mActor);
    nbt["fromDimension"] = static_cast<int>(mFromDimension);
    nbt["toDimension"]   = static_cast<int>(mToDimension);
}

class ActorChangeDimensionBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorChangeDimensionBeforeEvent> {};

// --- ActorChangeDimensionAfterEvent ---
Actor&        ActorChangeDimensionAfterEvent::getActor() const { return mActor; }
DimensionType ActorChangeDimensionAfterEvent::getFromDimension() const { return mFromDimension; }
DimensionType ActorChangeDimensionAfterEvent::getToDimension() const { return mToDimension; }

void ActorChangeDimensionAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["actor"]        = ll::event::serializeRefObj(mActor);
    nbt["fromDimension"] = static_cast<int>(mFromDimension);
    nbt["toDimension"]   = static_cast<int>(mToDimension);
}

class ActorChangeDimensionAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorChangeDimensionAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    ActorChangeDimensionHook,
    ll::memory::HookPriority::Normal,
    Level,
    &Level::$entityChangeDimension,
    void,
    class Actor&              entity,
    DimensionType             toId,
    std::optional<class Vec3> entityPos
) {
    try {
        DimensionType fromId = entity.getDimensionId();

        auto event = CauldronZero::event::ActorChangeDimensionBeforeEvent(entity, fromId, toId);


        ll::event::EventBus::getInstance().publish(event);


        if (event.isCancelled()) {

            return;
        }
        origin(entity, toId, entityPos);

        auto afterEvent = CauldronZero::event::ActorChangeDimensionAfterEvent(entity, fromId, toId);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "ActorChangeDimensionHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(entity, toId, entityPos);
    } catch (const std::exception& e) {
        logger.warn("ActorChangeDimensionHook 发生 C++ 异常: {}", e.what());
        origin(entity, toId, entityPos);
    } catch (...) {
        logger.warn("ActorChangeDimensionHook 发生未知异常！");
        origin(entity, toId, entityPos);
    }
}
void registerActorChangeDimensionEventHooks() { ActorChangeDimensionHook::hook(); }

void unregisterActorChangeDimensionEventHooks() { ActorChangeDimensionHook::unhook(); }
} // namespace CauldronZero::event
