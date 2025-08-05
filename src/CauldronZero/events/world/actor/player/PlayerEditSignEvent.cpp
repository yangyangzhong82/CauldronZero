#include "PlayerEditSignEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/network/ServerNetworkHandler.h"
#include "mc/network/packet/BlockActorDataPacket.h"
#include "mc/server/ServerPlayer.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/actor/SignBlockActor.h"


namespace CauldronZero::event {

// --- PlayerEditSignBeforeEvent ---

ServerPlayer&   PlayerEditSignBeforeEvent::getPlayer() const { return mPlayer; }
SignBlockActor& PlayerEditSignBeforeEvent::getSign() const { return mSign; }
std::string&    PlayerEditSignBeforeEvent::getNewFrontText() { return mNewFrontText; }
std::string&    PlayerEditSignBeforeEvent::getNewBackText() { return mNewBackText; }

void PlayerEditSignBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["player"]       = ll::event::serializeRefObj(mPlayer);
    nbt["sign"]         = ll::event::serializeRefObj(mSign);
    nbt["newFrontText"] = mNewFrontText;
    nbt["newBackText"]  = mNewBackText;
}

// --- PlayerEditSignAfterEvent ---

ServerPlayer&      PlayerEditSignAfterEvent::getPlayer() const { return mPlayer; }
SignBlockActor&    PlayerEditSignAfterEvent::getSign() const { return mSign; }
const std::string& PlayerEditSignAfterEvent::getOldFrontText() const { return mOldFrontText; }
const std::string& PlayerEditSignAfterEvent::getOldBackText() const { return mOldBackText; }

void PlayerEditSignAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["player"]       = ll::event::serializeRefObj(mPlayer);
    nbt["sign"]         = ll::event::serializeRefObj(mSign);
    nbt["oldFrontText"] = mOldFrontText;
    nbt["oldBackText"]  = mOldBackText;
}


LL_TYPE_INSTANCE_HOOK(
    PlayerEditSignEventHook,
    HookPriority::Normal,
    ServerNetworkHandler,
    &ServerNetworkHandler::$handle,
    void,
    NetworkIdentifier const&              pSource,
    std::shared_ptr<BlockActorDataPacket> pPacket
) {
    try {
        logger.debug("PlayerEditSignEventHook::hook: 尝试获取 NBT 数据.");
        auto& nbtData = pPacket->mData.get();
        if (nbtData.contains("id", Tag::Type::String)) {
            std::string id = nbtData["id"].get<StringTag>();
            logger.debug("PlayerEditSignEventHook::hook: NBT 数据包含 'id' 字段, id: {}", id);
            if (id == "Sign" || id == "HangingSign") {
                logger.debug("PlayerEditSignEventHook::hook: 'id' 为 'Sign' 或 'HangingSign'.");
                auto* player = thisFor<NetEventCallback>()->_getServerPlayer(pSource, pPacket->mSenderSubId);
                if (player) {
                    logger.debug("PlayerEditSignEventHook::hook: 成功获取到玩家对象.");
                    auto* signActor =
                        static_cast<SignBlockActor*>(player->getDimensionBlockSource().getBlockEntity(pPacket->mPos));
                    if (signActor) {
                        logger.debug("PlayerEditSignEventHook::hook: 成功获取到告示牌实体.");
                        const auto& oldFrontText = signActor->mTextFront->getMessage();
                        const auto& oldBackText  = signActor->mTextBack->getMessage();
                        logger.debug(
                            "PlayerEditSignEventHook::hook: 旧的前面文本: '{}', 旧的背面文本: '{}'",
                            oldFrontText,
                            oldBackText
                        );

                        const auto& newFrontTextData = nbtData["FrontText"].get<CompoundTag>();
                        const auto& newBackTextData  = nbtData["BackText"].get<CompoundTag>();

                        std::string newFrontText = newFrontTextData["Text"].get<StringTag>();
                        std::string newBackText  = newBackTextData["Text"].get<StringTag>();
                        logger.debug(
                            "PlayerEditSignEventHook::hook: 新的前面文本: '{}', 新的背面文本: '{}'",
                            newFrontText,
                            newBackText
                        );

                        logger.debug("PlayerEditSignEventHook::hook: 发布 PlayerEditSignBeforeEvent.");
                        auto beforeEvent = PlayerEditSignBeforeEvent(*player, *signActor, newFrontText, newBackText);
                        ll::event::EventBus::getInstance().publish(beforeEvent);
                        logger.debug("PlayerEditSignEventHook::hook: PlayerEditSignBeforeEvent 发布完成.");

                        if (beforeEvent.isCancelled()) {
                           // logger.info("PlayerEditSignEventHook::hook: PlayerEditSignBeforeEvent 被取消.");
                            return;
                        }

                        // Update the NBT with potentially modified text from the event
                        nbtData["FrontText"].get<CompoundTag>()["Text"] = StringTag(beforeEvent.getNewFrontText());
                        nbtData["BackText"].get<CompoundTag>()["Text"]  = StringTag(beforeEvent.getNewBackText());
                        logger.debug(
                            "PlayerEditSignEventHook::hook: NBT 数据已更新为事件修改后的文本. 新的前面文本: '{}', 新的背面文本: '{}'",
                            beforeEvent.getNewFrontText(),
                            beforeEvent.getNewBackText()
                        );

                        logger.debug("PlayerEditSignEventHook::hook: 调用原始处理函数.");
                        origin(pSource, pPacket);
                        logger.debug("PlayerEditSignEventHook::hook: 原始处理函数调用完成.");

                        logger.debug("PlayerEditSignEventHook::hook: 发布 PlayerEditSignAfterEvent.");
                        auto afterEvent = PlayerEditSignAfterEvent(*player, *signActor, oldFrontText, oldBackText);
                        ll::event::EventBus::getInstance().publish(afterEvent);
                        logger.debug("PlayerEditSignEventHook::hook: PlayerEditSignAfterEvent 发布完成.");
                        return;
                    } else {
                        logger.warn("PlayerEditSignEventHook::hook: 未能获取到告示牌实体.");
                    }
                } else {
                    logger.warn("PlayerEditSignEventHook::hook: 未能获取到玩家对象.");
                }
            } else {
                logger.debug("PlayerEditSignEventHook::hook: 'id' 不是 'Sign' 或 'HangingSign'.");
            }
        } else {
            logger.debug("PlayerEditSignEventHook::hook: NBT 数据不包含 'id' 字段.");
        }
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 PlayerEditSignEventHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
    } catch (const std::exception& e) {
        logger.error("标准异常在 PlayerEditSignEventHook::hook 中捕获: {}", e.what());
    } catch (...) {
        logger.error("未知异常在 PlayerEditSignEventHook::hook 中捕获");
    }
    logger.debug("PlayerEditSignEventHook::hook: 最终调用原始处理函数 (如果之前没有返回).");
    origin(pSource, pPacket);
}


void registerPlayerEditSignHooks() { PlayerEditSignEventHook::hook(); }

void unregisterPlayerEditSignHooks() { PlayerEditSignEventHook::unhook(); }

} // namespace CauldronZero::event
