#pragma once
#include "CauldronZero/events/world/block/ItemFrameBlockEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/memory/Hook.h"
#include "CauldronZero/Logger.h"
#include "ll/api/event/EventRefObjSerializer.h" // Added
#include "mc/nbt/CompoundTag.h"                 // Added
#include "mc/nbt/CompoundTagVariant.h"          // Added
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/ItemFrameBlock.h"
#include "mc/world/level/block/block_events/BlockPlayerInteractEvent.h"


namespace CauldronZero::event {

// --- Implementation for ItemFrameBlockAttackBeforeEvent ---
Player&         ItemFrameBlockAttackBeforeEvent::getPlayer() const { return mPlayer; }
BlockPos const& ItemFrameBlockAttackBeforeEvent::getPos() const { return mPos; }

void ItemFrameBlockAttackBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"] = ll::event::serializeRefObj(mPlayer);
    nbt["pos"]    = ListTag{mPos.x, mPos.y, mPos.z};
}

class ItemFrameBlockAttackBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, ItemFrameBlockAttackBeforeEvent> {};

// --- Implementation for ItemFrameBlockAttackAfterEvent ---
Player&         ItemFrameBlockAttackAfterEvent::getPlayer() const { return mPlayer; }
BlockPos const& ItemFrameBlockAttackAfterEvent::getPos() const { return mPos; }

void ItemFrameBlockAttackAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"] = ll::event::serializeRefObj(mPlayer);
    nbt["pos"]    = ListTag{mPos.x, mPos.y, mPos.z};
}

class ItemFrameBlockAttackAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, ItemFrameBlockAttackAfterEvent> {};


// --- Implementation for ItemFrameBlockUseBeforeEvent ---
Player& ItemFrameBlockUseBeforeEvent::getPlayer() const { return mEventData.mPlayer; }
Vec3    ItemFrameBlockUseBeforeEvent::getPos() const {
    if (mEventData.mHit->has_value()) {
        return mEventData.mHit->value();
    }
    return {};
}
BlockSource const& ItemFrameBlockUseBeforeEvent::getBlockSource() const { return mEventData.getBlockSource(); }

void ItemFrameBlockUseBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"]    = ll::event::serializeRefObj(mEventData.mPlayer);
    nbt["dimension"] = static_cast<int>(mEventData.getBlockSource().getDimensionId());
    nbt["blockPos"]  = ListTag{mEventData.mPos.get().x, mEventData.mPos.get().y, mEventData.mPos.get().z};
    nbt["face"]      = static_cast<int>(mEventData.mFace);
    if (mEventData.mHit->has_value()) {
        nbt["hitPos"] = ListTag{mEventData.mHit->value().x, mEventData.mHit->value().y, mEventData.mHit->value().z};
    }
}

class ItemFrameBlockUseBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, ItemFrameBlockUseBeforeEvent> {};

// --- Implementation for ItemFrameBlockUseAfterEvent ---
Player& ItemFrameBlockUseAfterEvent::getPlayer() const { return mEventData.mPlayer; }
Vec3    ItemFrameBlockUseAfterEvent::getPos() const {
    if (mEventData.mHit->has_value()) {
        return mEventData.mHit->value();
    }
    return {};
}
BlockSource const& ItemFrameBlockUseAfterEvent::getBlockSource() const { return mEventData.getBlockSource(); }

void ItemFrameBlockUseAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"]    = ll::event::serializeRefObj(mEventData.mPlayer);
    nbt["dimension"] = static_cast<int>(mEventData.getBlockSource().getDimensionId());
    nbt["blockPos"]  = ListTag{mEventData.mPos.get().x, mEventData.mPos.get().y, mEventData.mPos.get().z};
    nbt["face"]      = static_cast<int>(mEventData.mFace);
    if (mEventData.mHit->has_value()) {
        nbt["hitPos"] = ListTag{mEventData.mHit->value().x, mEventData.mHit->value().y, mEventData.mHit->value().z};
    }
}

class ItemFrameBlockUseAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, ItemFrameBlockUseAfterEvent> {};


// --- Hooks ---

LL_TYPE_INSTANCE_HOOK(
    FrameBlockEventHook,
    ll::memory::HookPriority::Normal,
    ItemFrameBlock,
    &ItemFrameBlock::$attack,
    bool,
    ::Player*         player,
    ::BlockPos const& pos
) {
    try {
        if (!player) {
            return origin(player, pos);
        }

        auto event = CauldronZero::event::ItemFrameBlockAttackBeforeEvent(*player, pos);
        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return false;
        }

        auto const result = origin(player, pos);

        auto afterEvent = CauldronZero::event::ItemFrameBlockAttackAfterEvent(*player, pos);
        ll::event::EventBus::getInstance().publish(afterEvent);

        return result;
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 FrameBlockEventHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(player, pos); // 返回原始行为
        throw;               // 重新抛出异常
    } catch (const std::exception& e) {
        logger.error("标准异常在 FrameBlockEventHook::hook 中捕获: {}", e.what());
        origin(player, pos); // 返回原始行为
        throw;               // 重新抛出异常
    } catch (...) {
        logger.error("未知异常在 FrameBlockEventHook::hook 中捕获");
        origin(player, pos); // 返回原始行为
        throw;               // 重新抛出异常
    }
}

LL_TYPE_INSTANCE_HOOK(
    FrameBlockEventHookB,
    ll::memory::HookPriority::Normal,
    ItemFrameBlock,
    &ItemFrameBlock::use,
    void,
    ::BlockEvents::BlockPlayerInteractEvent& eventData
) {
    try {
        auto event = CauldronZero::event::ItemFrameBlockUseBeforeEvent(eventData);
        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return; // Prevent original logic
        }

        origin(eventData);

        auto afterEvent = CauldronZero::event::ItemFrameBlockUseAfterEvent(eventData);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 FrameBlockEventHookB::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(eventData); // 返回原始行为
        throw;             // 重新抛出异常
    } catch (const std::exception& e) {
        logger.error("标准异常在 FrameBlockEventHookB::hook 中捕获: {}", e.what());
        origin(eventData); // 返回原始行为
        throw;             // 重新抛出异常
    } catch (...) {
        logger.error("未知异常在 FrameBlockEventHookB::hook 中捕获");
        origin(eventData); // 返回原始行为
        throw;             // 重新抛出异常
    }
}
void registerFrameBlockuseEventHooks() { FrameBlockEventHookB::hook(); }
void unregisterFrameBlockuseEventHooks() { FrameBlockEventHookB::unhook(); }
void registerFrameBlockattackEventHooks() { FrameBlockEventHook::hook(); }
void unregisterFrameBlockattackEventHooks() { FrameBlockEventHook::unhook(); }
} // namespace CauldronZero::event
