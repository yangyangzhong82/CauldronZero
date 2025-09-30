#include "MobHealthChangeEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/Mob.h" // 包含 Mob 的定义
#include "mc/world/attribute/HealthAttributeDelegate.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"


namespace CauldronZero::event {

// --- MobHealthChangeBeforeEvent ---
Mob&                     MobHealthChangeBeforeEvent::getMob() const { return mMob; }
float                    MobHealthChangeBeforeEvent::getOldValue() const { return mOldValue; }
float                    MobHealthChangeBeforeEvent::getNewValue() const { return mNewValue; }
::AttributeBuff const&   MobHealthChangeBeforeEvent::getBuff() const { return mBuff; }

void MobHealthChangeBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["mob"]       = ll::event::serializeRefObj(mMob);
    nbt["oldValue"]  = mOldValue;
    nbt["newValue"]  = mNewValue;
    nbt["buff"]      = ll::event::serializeRefObj(mBuff); 
}

class MobHealthChangeBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::MobHealthChangeBeforeEvent> {};

// --- MobHealthChangeAfterEvent ---
Mob&                     MobHealthChangeAfterEvent::getMob() const { return mMob; }
float                    MobHealthChangeAfterEvent::getOldValue() const { return mOldValue; }
float                    MobHealthChangeAfterEvent::getNewValue() const { return mNewValue; }
::AttributeBuff const&   MobHealthChangeAfterEvent::getBuff() const { return mBuff; }

void MobHealthChangeAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["mob"]           = ll::event::serializeRefObj(mMob);
    nbt["oldValue"]      = mOldValue;
    nbt["preChangeValue"] = mPreChangeValue;
    nbt["newValue"]      = mNewValue;
    nbt["buff"]          = ll::event::serializeRefObj(mBuff); 
}

class MobHealthChangeAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::MobHealthChangeAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    MobHealthChangeHook,
    ll::memory::HookPriority::Normal,
    HealthAttributeDelegate,
    &HealthAttributeDelegate::$change,
    float,
    float                  oldValue,
    float                  newValue,
    ::AttributeBuff const& buff
) {
    try {
        auto& mob = *mMob; 
        auto  beforeEvent = MobHealthChangeBeforeEvent(mob, oldValue, newValue, buff);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return oldValue; // 如果事件被取消，则返回旧值，阻止生命值更改
        }

        float result = origin(oldValue, newValue, buff);

        auto afterEvent = MobHealthChangeAfterEvent(mob, oldValue, newValue, result, buff); 
        ll::event::EventBus::getInstance().publish(afterEvent);

        return result;
    } catch (const SEH_Exception& e) {
        logger.warn(
            "MobHealthChangeHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(oldValue, newValue, buff);
    } catch (const std::exception& e) {
        logger.warn("MobHealthChangeHook 发生 C++ 异常: {}", e.what());
        return origin(oldValue, newValue, buff);
    } catch (...) {
        logger.warn("MobHealthChangeHook 发生未知异常！");
        return origin(oldValue, newValue, buff);
    }
}

void registerMobHealthChangeEventHooks() { MobHealthChangeHook::hook(); }
void unregisterMobHealthChangeEventHooks() { MobHealthChangeHook::unhook(); }

} // namespace CauldronZero::event
