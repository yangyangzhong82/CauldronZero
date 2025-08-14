#include "PlayerArmorStandSwapItemEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/ArmorStand.h"
#include "mc/world/actor/player/Player.h"
#include <ll/api/event/EventRefObjSerializer.h>

namespace CauldronZero::event {

Player& PlayerArmorStandSwapItemBeforeEvent::getPlayer() const { return mPlayer; }
ArmorStand& PlayerArmorStandSwapItemBeforeEvent::getArmorStand() const { return mArmorStand; }
::SharedTypes::Legacy::EquipmentSlot PlayerArmorStandSwapItemBeforeEvent::getSlot() const { return mSlot; }

void PlayerArmorStandSwapItemBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"]     = ll::event::serializeRefObj(mPlayer);
    nbt["armorStand"] = ll::event::serializeRefObj(mArmorStand);
    nbt["slot"]       = static_cast<int>(mSlot);
}

Player& PlayerArmorStandSwapItemAfterEvent::getPlayer() const { return mPlayer; }
ArmorStand& PlayerArmorStandSwapItemAfterEvent::getArmorStand() const { return mArmorStand; }
::SharedTypes::Legacy::EquipmentSlot PlayerArmorStandSwapItemAfterEvent::getSlot() const { return mSlot; }

void PlayerArmorStandSwapItemAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"]     = ll::event::serializeRefObj(mPlayer);
    nbt["armorStand"] = ll::event::serializeRefObj(mArmorStand);
    nbt["slot"]       = static_cast<int>(mSlot);
}


LL_TYPE_INSTANCE_HOOK(
    ArmorStandSwapItemHook,
    HookPriority::Normal,
    ArmorStand,
    &ArmorStand::_trySwapItem,
    bool,
    Player&                              player,
    ::SharedTypes::Legacy::EquipmentSlot slot
) {
    try {
        auto event = PlayerArmorStandSwapItemBeforeEvent(player, *this, slot);
        ll::event::EventBus::getInstance().publish(event);
        if (event.isCancelled()) {
            return false;
        }
        bool result = origin(player, slot);
        if (result) {
            auto afterEvent = PlayerArmorStandSwapItemAfterEvent(player, *this, slot);
            ll::event::EventBus::getInstance().publish(afterEvent);
        }
        return result;
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 ArmorStandSwapItemHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        return origin(player, slot);
    } catch (const std::exception& e) {
        logger.error("标准异常在 ArmorStandSwapItemHook::hook 中捕获: {}", e.what());
        return origin(player, slot);
    } catch (...) {
        logger.error("未知异常在 ArmorStandSwapItemHook::hook 中捕获");
        return origin(player, slot);
    }
}

void registerPlayerArmorStandSwapItemHooks() { ArmorStandSwapItemHook::hook(); }

void unregisterPlayerArmorStandSwapItemHooks() { ArmorStandSwapItemHook::unhook(); }

} // namespace CauldronZero::event