#include "DragonRespawnEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/dimension/end/EndDragonFight.h"
#include "mc/world/level/dimension/end/RespawnAnimation.h" // 包含 RespawnAnimation 枚举
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"

namespace CauldronZero::event {

// --- DragonRespawnBeforeEvent ---
EndDragonFight&   DragonRespawnBeforeEvent::getEndDragonFight() const { return mEndDragonFight; }
RespawnAnimation& DragonRespawnBeforeEvent::getStage() const { return mStage; }

void DragonRespawnBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["endDragonFight"] = ll::event::serializeRefObj(mEndDragonFight);
    nbt["stage"]          = static_cast<int>(mStage);
}

class DragonRespawnBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::DragonRespawnBeforeEvent> {};

// --- DragonRespawnAfterEvent ---
EndDragonFight&   DragonRespawnAfterEvent::getEndDragonFight() const { return mEndDragonFight; }
RespawnAnimation& DragonRespawnAfterEvent::getStage() const { return mStage; }

void DragonRespawnAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["endDragonFight"] = ll::event::serializeRefObj(mEndDragonFight);
    nbt["stage"]          = static_cast<int>(mStage);
}

class DragonRespawnAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::DragonRespawnAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    DragonRespawnHook,
    HookPriority::Normal,
    EndDragonFight,
    &EndDragonFight::_setRespawnStage,
    void,
    RespawnAnimation stage
) {
    try {
        auto& endDragonFight = *this;
        auto  beforeEvent = DragonRespawnBeforeEvent(endDragonFight, stage);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(stage);

        auto afterEvent = DragonRespawnAfterEvent(endDragonFight, stage);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "DragonRespawnHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(stage);
    } catch (const std::exception& e) {
        logger.warn("DragonRespawnHook 发生 C++ 异常: {}", e.what());
        origin(stage);
    } catch (...) {
        logger.warn("DragonRespawnHook 发生未知异常！");
        origin(stage);
    }
}

void registerDragonRespawnEventHooks() { DragonRespawnHook::hook(); }
void unregisterDragonRespawnEventHooks() { DragonRespawnHook::unhook(); }

} // namespace CauldronZero::event
