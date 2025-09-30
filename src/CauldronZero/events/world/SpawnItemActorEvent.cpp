#include "SpawnItemActorEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/BedrockSpawner.h"
#include "mc/world/actor/item/ItemActor.h" 
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"

namespace CauldronZero::event {

// --- SpawnItemActorBeforeEvent ---
BlockSource& SpawnItemActorBeforeEvent::getRegion() const { return mRegion; }
ItemStack const& SpawnItemActorBeforeEvent::getInst() const { return mInst; }
Actor* SpawnItemActorBeforeEvent::getSpawner() const { return mSpawner; }
Vec3 const& SpawnItemActorBeforeEvent::getPos() const { return mPos; }
int SpawnItemActorBeforeEvent::getThrowTime() const { return mThrowTime; }

void SpawnItemActorBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["region"] = ll::event::serializeRefObj(mRegion);
    nbt["itemStack"] = ll::event::serializeRefObj(mInst);
    if (mSpawner) {
        nbt["spawner"] = ll::event::serializeRefObj(*mSpawner);
    }
    nbt["pos_x"] = mPos.x;
    nbt["pos_y"] = mPos.y;
    nbt["pos_z"] = mPos.z;
    nbt["throwTime"] = mThrowTime;
}

class SpawnItemActorBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::SpawnItemActorBeforeEvent> {};

// --- SpawnItemActorAfterEvent ---
BlockSource& SpawnItemActorAfterEvent::getRegion() const { return mRegion; }
ItemStack const& SpawnItemActorAfterEvent::getInst() const { return mInst; }
Actor* SpawnItemActorAfterEvent::getSpawner() const { return mSpawner; }
Vec3 const& SpawnItemActorAfterEvent::getPos() const { return mPos; }
int SpawnItemActorAfterEvent::getThrowTime() const { return mThrowTime; }
ItemActor* SpawnItemActorAfterEvent::getItemActor() const { return mItemActor; }

void SpawnItemActorAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["region"] = ll::event::serializeRefObj(mRegion);
    nbt["itemStack"] = ll::event::serializeRefObj(mInst);
    if (mSpawner) {
        nbt["spawner"] = ll::event::serializeRefObj(*mSpawner);
    }
    nbt["pos_x"] = mPos.x;
    nbt["pos_y"] = mPos.y;
    nbt["pos_z"] = mPos.z;
    nbt["throwTime"] = mThrowTime;
    if (mItemActor) {
        nbt["itemActor"] = ll::event::serializeRefObj(*mItemActor);
    }
}

class SpawnItemActorAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::SpawnItemActorAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    SpawnItemActorHook,
    HookPriority::Normal,
    BedrockSpawner,
    &BedrockSpawner::$spawnItem,
    ItemActor*,
    ::BlockSource&     region,
    ::ItemStack const& inst,
    ::Actor*           spawner,
    ::Vec3 const&      pos,
    int                throwTime
) {
    try {
        auto beforeEvent = SpawnItemActorBeforeEvent(region, inst, spawner, pos, throwTime);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return nullptr;
        }

        ItemActor* itemActor = origin(region, inst, spawner, pos, throwTime);

        auto afterEvent = SpawnItemActorAfterEvent(region, inst, spawner, pos, throwTime, itemActor);
        ll::event::EventBus::getInstance().publish(afterEvent);

        return itemActor;
    } catch (const SEH_Exception& e) {
        logger.warn(
            "SpawnItemActorHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(region, inst, spawner, pos, throwTime);
    } catch (const std::exception& e) {
        logger.warn("SpawnItemActorHook 发生 C++ 异常: {}", e.what());
        return origin(region, inst, spawner, pos, throwTime);
    } catch (...) {
        logger.warn("SpawnItemActorHook 发生未知异常！");
        return origin(region, inst, spawner, pos, throwTime);
    }
}

void registerSpawnItemActorEventHooks() { SpawnItemActorHook::hook(); }
void unregisterSpawnItemActorEventHooks() { SpawnItemActorHook::unhook(); }

} // namespace CauldronZero::event
