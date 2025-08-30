#include "ActorPressurePlateTriggerEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/BasePressurePlateBlock.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/block/Block.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include <ll/api/event/EventRefObjSerializer.h>

namespace CauldronZero::event {

// --- ActorPressurePlateTriggerBeforeEvent ---
Actor&        ActorPressurePlateTriggerBeforeEvent::getActor() const { return mActor; }
Block const&  ActorPressurePlateTriggerBeforeEvent::getBlock() const { return mBlock; }
BlockPos const& ActorPressurePlateTriggerBeforeEvent::getPos() const { return mPos; }

void ActorPressurePlateTriggerBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["actor"] = ll::event::serializeRefObj(mActor);
    nbt["block"] = ll::event::serializeRefObj(mBlock);
    nbt["pos_x"] = mPos.x;
    nbt["pos_y"] = mPos.y;
    nbt["pos_z"] = mPos.z;
}

class ActorPressurePlateTriggerBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorPressurePlateTriggerBeforeEvent> {};

// --- ActorPressurePlateTriggerAfterEvent ---
Actor&        ActorPressurePlateTriggerAfterEvent::getActor() const { return mActor; }
Block const&  ActorPressurePlateTriggerAfterEvent::getBlock() const { return mBlock; }
BlockPos const& ActorPressurePlateTriggerAfterEvent::getPos() const { return mPos; }

void ActorPressurePlateTriggerAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["actor"] = ll::event::serializeRefObj(mActor);
    nbt["block"] = ll::event::serializeRefObj(mBlock);
    nbt["pos_x"] = mPos.x;
    nbt["pos_y"] = mPos.y;
    nbt["pos_z"] = mPos.z;
}

class ActorPressurePlateTriggerAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorPressurePlateTriggerAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    PressurePlateTriggerHook,
    ll::memory::HookPriority::Normal,
    BasePressurePlateBlock,
    &BasePressurePlateBlock::$shouldTriggerEntityInside,
    bool,
    BlockSource&    region,
    BlockPos const& pos,
    Actor&          entity
) {
    try {
        Block const& block = region.getBlock(pos);

        auto beforeEvent = CauldronZero::event::ActorPressurePlateTriggerBeforeEvent(entity, block, pos);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return false;
        }

        bool result = origin(region, pos, entity);

        if (result) {
            auto afterEvent = CauldronZero::event::ActorPressurePlateTriggerAfterEvent(entity, block, pos);
            ll::event::EventBus::getInstance().publish(afterEvent);
        }
        return result;
    } catch (const SEH_Exception& e) {
        logger.warn(
            "PressurePlateTriggerHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(region, pos, entity);
    } catch (const std::exception& e) {
        logger.warn("PressurePlateTriggerHook 发生 C++ 异常: {}", e.what());
        return origin(region, pos, entity);
    } catch (...) {
        logger.warn("PressurePlateTriggerHook 发生未知异常！");
        return origin(region, pos, entity);
    }
}

void registerActorPressurePlateTriggerEventHooks() { PressurePlateTriggerHook::hook(); }

void unregisterActorPressurePlateTriggerEventHooks() { PressurePlateTriggerHook::unhook(); }

} // namespace CauldronZero::event
