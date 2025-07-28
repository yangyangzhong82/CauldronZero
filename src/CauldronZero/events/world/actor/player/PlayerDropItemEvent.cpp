#include "PlayerDropItemEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/player/Inventory.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/actor/player/PlayerInventory.h"
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include <mc/world/inventory/transaction/ComplexInventoryTransaction.h>
#include <mc/world/inventory/transaction/InventoryAction.h>
#include <mc/world/inventory/transaction/InventorySourceType.h>
#include <mc/world/inventory/transaction/InventoryTransaction.h>


namespace CauldronZero::event {

Player& PlayerDropItemBeforeEvent::getPlayer() const { return mPlayer; }
ItemStack& PlayerDropItemBeforeEvent::getItem() { return mItem; }
const ItemStack& PlayerDropItemBeforeEvent::getItem() const { return mItem; }

void PlayerDropItemBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"] = ll::event::serializeRefObj(mPlayer);
    nbt["item"]   = ll::event::serializeRefObj(mItem);
}

Player& PlayerDropItemAfterEvent::getPlayer() const { return mPlayer; }
const ItemStack& PlayerDropItemAfterEvent::getItem() const { return mItem; }

void PlayerDropItemAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"] = ll::event::serializeRefObj(mPlayer);
    nbt["item"]   = ll::event::serializeRefObj(mItem);
}

LL_TYPE_INSTANCE_HOOK(
    PlayerDropItemEventHook1,
    HookPriority::Normal,
    Player,
    &Player::$drop,
    bool,
    ItemStack const& item,
    bool             randomly
) {
    try {
        auto event = PlayerDropItemBeforeEvent(*this, item);
        ll::event::EventBus::getInstance().publish(event);
        if (event.isCancelled()) {
            return false;
        }
        bool result = origin(event.getItem(), randomly);
        if (result) {
            auto afterEvent = PlayerDropItemAfterEvent(*this, event.getItem());
            ll::event::EventBus::getInstance().publish(afterEvent);
        }
        return result;
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 PlayerDropItemEventHook1::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        return origin(item, randomly);
    } catch (const std::exception& e) {
        logger.error("标准异常在 PlayerDropItemEventHook1::hook 中捕获: {}", e.what());
        return origin(item, randomly);
    } catch (...) {
        logger.error("未知异常在 PlayerDropItemEventHook1::hook 中捕获");
        return origin(item, randomly);
    }
}

LL_TYPE_INSTANCE_HOOK(
    PlayerDropItemEventHook2,
    HookPriority::Normal,
    ComplexInventoryTransaction,
    &ComplexInventoryTransaction::$handle,
    InventoryTransactionError,
    Player& player,
    bool    isSenderAuthority
) {
    if (mType == ComplexInventoryTransaction::Type::NormalTransaction) {
        try {
            InventorySource source{
                InventorySourceType::ContainerInventory,
                ContainerID::Inventory,
                InventorySource::InventorySourceFlags::NoFlag
            };
            auto& actions = mTransaction->getActions(source);
            if (actions.size() != 1) {
                return origin(player, isSenderAuthority);
            }
            auto& item = player.mInventory->mInventory->getItem(actions[0].mSlot);

       
            auto event = PlayerDropItemBeforeEvent(player, item);
            ll::event::EventBus::getInstance().publish(event);
            if (event.isCancelled()) {
                return InventoryTransactionError::AuthorityMismatch; // 拦截
            }
            InventoryTransactionError result = origin(player, isSenderAuthority);
            if (result == InventoryTransactionError::NoError) {
                auto afterEvent = PlayerDropItemAfterEvent(player, event.getItem());
                ll::event::EventBus::getInstance().publish(afterEvent);
            }
            return result;
        } catch (const SEH_Exception& e) {
            logger.error(
                "SEH 异常在 PlayerDropItemEventHook2::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
                e.getSehCode(),
                e.getExceptionAddress(),
                e.what()
            );
            return origin(player, isSenderAuthority);
        } catch (const std::exception& e) {
            logger.error("标准异常在 PlayerDropItemEventHook2::hook 中捕获: {}", e.what());
            return origin(player, isSenderAuthority);
        } catch (...) {
            logger.error("未知异常在 PlayerDropItemEventHook2::hook 中捕获");
            return origin(player, isSenderAuthority);
        }
    }
    return origin(player, isSenderAuthority);
}

void registerPlayerDropItemHooks() {
    PlayerDropItemEventHook1::hook();
    PlayerDropItemEventHook2::hook();
}

void unregisterPlayerDropItemHooks() {
    PlayerDropItemEventHook1::unhook();
    PlayerDropItemEventHook2::unhook();
}

} // namespace CauldronZero::event
