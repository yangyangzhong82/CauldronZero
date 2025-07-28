#include "PlayerInteractEntityEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/player/Player.h"
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>

namespace CauldronZero::event {

Player& PlayerInteractEntityBeforeEvent::getPlayer() const { return mPlayer; }
Actor& PlayerInteractEntityBeforeEvent::getEntity() const { return mEntity; }
Vec3 const& PlayerInteractEntityBeforeEvent::getInteractLocation() const { return mInteractLocation; }

void PlayerInteractEntityBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"]          = ll::event::serializeRefObj(mPlayer);
    nbt["entity"]          = ll::event::serializeRefObj(mEntity);
    nbt["interactLocation"] = mInteractLocation.toString();
}

Player& PlayerInteractEntityAfterEvent::getPlayer() const { return mPlayer; }
Actor& PlayerInteractEntityAfterEvent::getEntity() const { return mEntity; }
Vec3 const& PlayerInteractEntityAfterEvent::getInteractLocation() const { return mInteractLocation; }

void PlayerInteractEntityAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"]          = ll::event::serializeRefObj(mPlayer);
    nbt["entity"]          = ll::event::serializeRefObj(mEntity);
    nbt["interactLocation"] = mInteractLocation.toString();
}

LL_TYPE_INSTANCE_HOOK(
    PlayerInteractEntityEventHook,
    HookPriority::Normal,
    Player,
    &Player::interact,
    bool,
    ::Actor&      actor,
    ::Vec3 const& location
) {
    try {
        auto event = PlayerInteractEntityBeforeEvent(*this, actor, location);
        ll::event::EventBus::getInstance().publish(event);
        if (event.isCancelled()) {
            return false;
        }
        bool result = origin(actor, location);
        if (result) {
            auto afterEvent = PlayerInteractEntityAfterEvent(*this, actor, location);
            ll::event::EventBus::getInstance().publish(afterEvent);
        }
        return result;
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 PlayerInteractEntityEventHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        return origin(actor, location);
    } catch (const std::exception& e) {
        logger.error("标准异常在 PlayerInteractEntityEventHook::hook 中捕获: {}", e.what());
        return origin(actor, location);
    } catch (...) {
        logger.error("未知异常在 PlayerInteractEntityEventHook::hook 中捕获");
        return origin(actor, location);
    }
}

void registerPlayerInteractEntityHooks() { PlayerInteractEntityEventHook::hook(); }

void unregisterPlayerInteractEntityHooks() { PlayerInteractEntityEventHook::unhook(); }

} // namespace CauldronZero::event
