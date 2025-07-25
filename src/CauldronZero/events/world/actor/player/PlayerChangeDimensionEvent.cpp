

#include "PlayerChangeDimensionEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/Actor.h" 
#include "mc/world/actor/player/Player.h" 
#include "mc/world/level/dimension/Dimension.h" 
#include "mc/world/level/ChangeDimensionRequest.h"
#include "mc/world/level/Level.h"
#include "mc\world\level\PlayerDimensionTransferManager.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include "mc/nbt/CompoundTag.h" 
#include "mc/nbt/CompoundTagVariant.h" 
#include <ll/api/event/EventRefObjSerializer.h> 


namespace CauldronZero::event {

Player&       PlayerChangeDimensionBeforeEvent::getPlayer() const { return mPlayer; }
DimensionType PlayerChangeDimensionBeforeEvent::getFromDimension() const { return mFromDimension; }
DimensionType PlayerChangeDimensionBeforeEvent::getToDimension() const { return mToDimension; }

void PlayerChangeDimensionBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"]        = ll::event::serializeRefObj(mPlayer);
    nbt["fromDimension"] = static_cast<int>(mFromDimension);
    nbt["toDimension"]   = static_cast<int>(mToDimension);
}

class PlayerChangeDimensionBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::PlayerChangeDimensionBeforeEvent> {
};

// --- PlayerChangeDimensionAfterEvent ---
Player&       PlayerChangeDimensionAfterEvent::getPlayer() const { return mPlayer; }
DimensionType PlayerChangeDimensionAfterEvent::getFromDimension() const { return mFromDimension; }
DimensionType PlayerChangeDimensionAfterEvent::getToDimension() const { return mToDimension; }

void PlayerChangeDimensionAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"]        = ll::event::serializeRefObj(mPlayer);
    nbt["fromDimension"] = static_cast<int>(mFromDimension);
    nbt["toDimension"]   = static_cast<int>(mToDimension);
}

class PlayerChangeDimensionAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::PlayerChangeDimensionAfterEvent> {
};


LL_TYPE_INSTANCE_HOOK(
    PlayerChangeDimensionHook,
    ll::memory::HookPriority::Normal,
    Level,
    &Level::$requestPlayerChangeDimension,
    void,
    Player&                  player,
    ChangeDimensionRequest&& changeRequest
) {
    try {
        DimensionType fromId = player.getDimensionId();      // 获取玩家当前维度
        DimensionType toId   = changeRequest.mToDimensionId; // 获取目标维度

        auto event = CauldronZero::event::PlayerChangeDimensionBeforeEvent(player, fromId, toId);


        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return;
        }
        origin(player, std::move(changeRequest));

        auto afterEvent = CauldronZero::event::PlayerChangeDimensionAfterEvent(player, fromId, toId);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 PlayerChangeDimensionHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(player, std::move(changeRequest)); // 返回原始行为
        throw;                                    // 重新抛出异常
    } catch (const std::exception& e) {
        logger.error("标准异常在 PlayerChangeDimensionHook::hook 中捕获: {}", e.what());
        origin(player, std::move(changeRequest)); // 返回原始行为
        throw;                                    // 重新抛出异常
    } catch (...) {
        logger.error("未知异常在 PlayerChangeDimensionHook::hook 中捕获");
        origin(player, std::move(changeRequest)); // 返回原始行为
        throw;                                    // 重新抛出异常
    }
}
void registerPlayerChangeDimensionHooks() { PlayerChangeDimensionHook::hook(); }

void unregisterPlayerChangeDimensionHooks() { PlayerChangeDimensionHook::unhook(); }
} // namespace CauldronZero::event
