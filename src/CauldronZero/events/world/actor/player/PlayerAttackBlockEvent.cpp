#include "PlayerAttackBlockEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/network/ServerPlayerBlockUseHandler.h"
#include "mc/world/level/block/Block.h"
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>

namespace CauldronZero::event {

ServerPlayer&   PlayerAttackBlockBeforeEvent::getPlayer() const { return mPlayer; }
const BlockPos& PlayerAttackBlockBeforeEvent::getBlockPos() const { return mBlockPos; }
int             PlayerAttackBlockBeforeEvent::getFace() const { return mFace; }

void PlayerAttackBlockBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"]   = ll::event::serializeRefObj(mPlayer);
    nbt["blockPos"] = mBlockPos.toString();
    nbt["face"]     = mFace;
}

ServerPlayer&   PlayerAttackBlockAfterEvent::getPlayer() const { return mPlayer; }
const BlockPos& PlayerAttackBlockAfterEvent::getBlockPos() const { return mBlockPos; }
int             PlayerAttackBlockAfterEvent::getFace() const { return mFace; }

void PlayerAttackBlockAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"]   = ll::event::serializeRefObj(mPlayer);
    nbt["blockPos"] = mBlockPos.toString();
    nbt["face"]     = mFace;
}

LL_STATIC_HOOK(
    StartDestroyBlockHook,
    HookPriority::Normal,
    &ServerPlayerBlockUseHandler::onStartDestroyBlock,
    void,
    ServerPlayer&   player,
    const BlockPos& pos,
    int             face
) {
    try {
        logger.debug("PlayerAttackBlockBeforeEven被激活");
        auto event = PlayerAttackBlockBeforeEvent(player, pos, face);
        ll::event::EventBus::getInstance().publish(event);
        if (event.isCancelled()) {
            return;
        }
        origin(player, pos, face);
        auto afterEvent = PlayerAttackBlockAfterEvent(player, pos, face);
        ll::event::EventBus::getInstance().publish(afterEvent);
        logger.debug("PlayerAttackBlockBeforeEvent处理完成参数: pos={}, face={}", pos.toString(), face);
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 StartDestroyBlockHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(player, pos, face);
        throw;
    } catch (const std::exception& e) {
        logger.error("标准异常在 StartDestroyBlockHook::hook 中捕获: {}", e.what());
        origin(player, pos, face);
        throw;
    } catch (...) {
        logger.error("未知异常在 StartDestroyBlockHook::hook 中捕获");
        origin(player, pos, face);
        throw;
    }
}

void registerPlayerAttackBlockHooks() { StartDestroyBlockHook::hook(); }

void unregisterPlayerAttackBlockHooks() { StartDestroyBlockHook::unhook(); }

} // namespace CauldronZero::event
