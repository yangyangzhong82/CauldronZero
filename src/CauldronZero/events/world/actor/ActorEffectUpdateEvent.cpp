#include "ActorEffectUpdateEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/effect/MobEffectInstance.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"


namespace CauldronZero::event {

// --- ActorEffectAddBeforeEvent ---
Actor&             ActorEffectAddBeforeEvent::getActor() const { return mActor; }
MobEffectInstance& ActorEffectAddBeforeEvent::getEffect() const { return mEffect; }

void ActorEffectAddBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["actor"] = ll::event::serializeRefObj(mActor);
    nbt["effect"] = ll::event::serializeRefObj(mEffect);
}

class ActorEffectAddBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorEffectAddBeforeEvent> {};

// --- ActorEffectAddAfterEvent ---
Actor&             ActorEffectAddAfterEvent::getActor() const { return mActor; }
MobEffectInstance& ActorEffectAddAfterEvent::getEffect() const { return mEffect; }

void ActorEffectAddAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["actor"] = ll::event::serializeRefObj(mActor);
    nbt["effect"] = ll::event::serializeRefObj(mEffect);
}

class ActorEffectAddAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorEffectAddAfterEvent> {};

// --- ActorEffectRemoveBeforeEvent ---
Actor&             ActorEffectRemoveBeforeEvent::getActor() const { return mActor; }
MobEffectInstance& ActorEffectRemoveBeforeEvent::getEffect() const { return mEffect; }

void ActorEffectRemoveBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["actor"] = ll::event::serializeRefObj(mActor);
    nbt["effect"] = ll::event::serializeRefObj(mEffect);
}

class ActorEffectRemoveBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorEffectRemoveBeforeEvent> {};

// --- ActorEffectRemoveAfterEvent ---
Actor&             ActorEffectRemoveAfterEvent::getActor() const { return mActor; }
MobEffectInstance& ActorEffectRemoveAfterEvent::getEffect() const { return mEffect; }

void ActorEffectRemoveAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["actor"] = ll::event::serializeRefObj(mActor);
    nbt["effect"] = ll::event::serializeRefObj(mEffect);
}

class ActorEffectRemoveAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorEffectRemoveAfterEvent> {};

// --- ActorEffectUpdateBeforeEvent ---
Actor&             ActorEffectUpdateBeforeEvent::getActor() const { return mActor; }
MobEffectInstance& ActorEffectUpdateBeforeEvent::getEffect() const { return mEffect; }

void ActorEffectUpdateBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["actor"] = ll::event::serializeRefObj(mActor);
    nbt["effect"] = ll::event::serializeRefObj(mEffect);
}

class ActorEffectUpdateBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorEffectUpdateBeforeEvent> {};

// --- ActorEffectUpdateAfterEvent ---
Actor&             ActorEffectUpdateAfterEvent::getActor() const { return mActor; }
MobEffectInstance& ActorEffectUpdateAfterEvent::getEffect() const { return mEffect; }

void ActorEffectUpdateAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["actor"] = ll::event::serializeRefObj(mActor);
    nbt["effect"] = ll::event::serializeRefObj(mEffect);
}

class ActorEffectUpdateAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorEffectUpdateAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    EffectUpdateHook,
    HookPriority::Normal,
    Actor,
    &Actor::onEffectUpdated,
    void,
    MobEffectInstance& effect
) {
    try {
        auto& actor = *this;
        auto  beforeEvent = ActorEffectUpdateBeforeEvent(actor, effect);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(effect);

        auto afterEvent = ActorEffectUpdateAfterEvent(actor, effect);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "EffectUpdateHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(effect);
    } catch (const std::exception& e) {
        logger.warn("EffectUpdateHook 发生 C++ 异常: {}", e.what());
        origin(effect);
    } catch (...) {
        logger.warn("EffectUpdateHook 发生未知异常！");
        origin(effect);
    }
}

LL_TYPE_INSTANCE_HOOK(
    RemoveEffectHook,
    HookPriority::Normal,
    Actor,
    &Actor::$onEffectRemoved,
    void,
    ::MobEffectInstance& effect
) {
    try {
        auto& actor = *this;
        auto  beforeEvent = ActorEffectRemoveBeforeEvent(actor, effect);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(effect);

        auto afterEvent = ActorEffectRemoveAfterEvent(actor, effect);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "RemoveEffectHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(effect);
    } catch (const std::exception& e) {
        logger.warn("RemoveEffectHook 发生 C++ 异常: {}", e.what());
        origin(effect);
    } catch (...) {
        logger.warn("RemoveEffectHook 发生未知异常！");
        origin(effect);
    }
}

LL_TYPE_INSTANCE_HOOK(
    AddEffectHook,
    HookPriority::Normal,
    Actor,
    &Actor::addEffect,
    void,
    ::MobEffectInstance const& effect
) {
    try {
        auto& actor = *this;
        // addEffect 的 effect 参数是 const&，但事件需要 MobEffectInstance&。
        // 这里创建一个可修改的副本传递给事件。
        MobEffectInstance mutableEffect = effect;
        auto  beforeEvent = ActorEffectAddBeforeEvent(actor, mutableEffect);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        // 如果事件没有取消，则使用原始的 effect 参数调用 origin
        origin(effect);

        auto afterEvent = ActorEffectAddAfterEvent(actor, mutableEffect);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "AddEffectHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(effect);
    } catch (const std::exception& e) {
        logger.warn("AddEffectHook 发生 C++ 异常: {}", e.what());
        origin(effect);
    } catch (...) {
        logger.warn("AddEffectHook 发生未知异常！");
        origin(effect);
    }
}

void registerActorEffectUpdateEventHooks() {
    EffectUpdateHook::hook();
    RemoveEffectHook::hook();
    AddEffectHook::hook();
}

void unregisterActorEffectUpdateEventHooks() {
    EffectUpdateHook::unhook();
    RemoveEffectHook::unhook();
    AddEffectHook::unhook();
}

} // namespace CauldronZero::event
