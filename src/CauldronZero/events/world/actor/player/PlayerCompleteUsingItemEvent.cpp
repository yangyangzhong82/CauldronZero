#include "PlayerCompleteUsingItemEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/item/ItemStack.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"

namespace CauldronZero::event {

// --- PlayerCompleteUsingItemBeforeEvent ---
Player&          PlayerCompleteUsingItemBeforeEvent::getPlayer() const { return mPlayer; }
ItemStack const& PlayerCompleteUsingItemBeforeEvent::getItem() const { return mItem; }

void PlayerCompleteUsingItemBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"] = ll::event::serializeRefObj(mPlayer);
    nbt["item"]   = ll::event::serializeRefObj(mItem);
}

class PlayerCompleteUsingItemBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::PlayerCompleteUsingItemBeforeEvent> {};

// --- PlayerCompleteUsingItemAfterEvent ---
Player&          PlayerCompleteUsingItemAfterEvent::getPlayer() const { return mPlayer; }
ItemStack const& PlayerCompleteUsingItemAfterEvent::getItem() const { return mItem; }

void PlayerCompleteUsingItemAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"] = ll::event::serializeRefObj(mPlayer);
    nbt["item"]   = ll::event::serializeRefObj(mItem);
}

class PlayerCompleteUsingItemAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::PlayerCompleteUsingItemAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    CompleteUsingItemHook,
    HookPriority::Normal,
    Player,
    &Player::completeUsingItem,
    void
) {
    try {
        auto& player = *this;
        // completeUsingItem 没有直接提供 ItemStack 参数，需要从玩家当前使用的物品中获取
        ItemStack const& item = player.getCarriedItem(); // 假设 getCarriedItem() 返回当前使用的物品

        auto beforeEvent = PlayerCompleteUsingItemBeforeEvent(player, item);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin();

        auto afterEvent = PlayerCompleteUsingItemAfterEvent(player, item);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "CompleteUsingItemHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin();
    } catch (const std::exception& e) {
        logger.warn("CompleteUsingItemHook 发生 C++ 异常: {}", e.what());
        origin();
    } catch (...) {
        logger.warn("CompleteUsingItemHook 发生未知异常！");
        origin();
    }
}

void registerPlayerCompleteUsingItemEventHooks() { CompleteUsingItemHook::hook(); }
void unregisterPlayerCompleteUsingItemEventHooks() { CompleteUsingItemHook::unhook(); }

} // namespace CauldronZero::event
