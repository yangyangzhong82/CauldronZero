#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/memory/Hook.h"
#include "mc/certificates/identity/LegacyMultiplayerToken.h"
#include <ll/api/service/Bedrock.h>
#include <mc/network/ConnectionRequest.h>
#include <mc/network/NetworkIdentifier.h>
#include <mc/network/ServerNetworkHandler.h>
#include <mc/network/packet/LoginPacket.h>
#include <mc/platform/UUID.h>
#include "CauldronZero/logger.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/events/server/ClientLoginEvent.h"

namespace CauldronZero::event {

ConnectionRequest& ClientLoginBeforeEvent::getConnectionRequest() const { return mConnectionRequest; }
const std::string& ClientLoginBeforeEvent::getPlayerName() const { return mPlayerName; }

const NetworkIdentifier& ClientLoginBeforeEvent::getNetworkIdentifier() const { return mNetworkIdentifier; }

void ClientLoginBeforeEvent::disconnect(const std::string& message) const {
    ll::service::getServerNetworkHandler()
        ->disconnectClient(mNetworkIdentifier, Connection::DisconnectFailReason::Kicked, message, std::nullopt, false);
}

std::string ClientLoginBeforeEvent::getIp() const { return mNetworkIdentifier.getIPAndPort(); }

std::string ClientLoginBeforeEvent::getXuid() const {
    return mConnectionRequest.mLegacyMultiplayerToken->getXuid(false);
}

mce::UUID ClientLoginBeforeEvent::getUuid() const {
    return mConnectionRequest.mLegacyMultiplayerToken->getIdentity();
}

std::string ClientLoginBeforeEvent::getDeviceId() const { return mConnectionRequest.getDeviceId(); }

// Emitter 是一个空类，它通过其静态变量 `reg`，负责在程序启动时
// 将 `ClientLoginBeforeEvent` 事件注册到 EventBus 中。
class ClientLoginBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ClientLoginBeforeEvent> {};


LL_TYPE_INSTANCE_HOOK(
    ClientLoginEventHook,
    HookPriority::Normal,
    ServerNetworkHandler,
    &ServerNetworkHandler::$handle,
    void,
    NetworkIdentifier const&     pSource,
    std::shared_ptr<LoginPacket> pPacket
) {
    try {
        // 先调用原始函数，确保所有信息都已加载
        origin(pSource, pPacket);

        auto* connReq = pPacket->mConnectionRequest.get();
        if (connReq) {
            auto name = connReq->mLegacyMultiplayerToken->getIdentityName();

       
            auto event = CauldronZero::event::ClientLoginBeforeEvent(*connReq, name, pSource);

            ll::event::EventBus::getInstance().publish(event);


            auto ip             = pSource.getIPAndPort();
            auto ServerAuthXuid = connReq->mLegacyMultiplayerToken->getXuid(false);
            auto ClientAuthXuid = connReq->mLegacyMultiplayerToken->getXuid(true);
            auto uuid           = connReq->mLegacyMultiplayerToken->getIdentity().asString();
            auto deviceId       = connReq->getDeviceId();

            logger.debug("Player connecting:");
            logger.debug("  IP: {}", ip);
            logger.debug("  XUID: {}", ServerAuthXuid);
            logger.debug("  UUID: {}", uuid);
            logger.debug("  Device ID: {}", deviceId);
            logger.debug("  Name: {}", name);
            logger.debug("  Client XUID: {}", ClientAuthXuid);
            
        }
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 ClientLoginEventHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(pSource, pPacket); // 返回原始行为
        throw;                    // 重新抛出异常
    } catch (const std::exception& e) {
        logger.error("标准异常在 ClientLoginEventHook::hook 中捕获: {}", e.what());
        origin(pSource, pPacket); // 返回原始行为
        throw;                    // 重新抛出异常
    } catch (...) {
        logger.error("未知异常在 ClientLoginEventHook::hook 中捕获");
        origin(pSource, pPacket); // 返回原始行为
        throw;                    // 重新抛出异常
    }
}
void registerClientLoginEventHooks() { ClientLoginEventHook::hook(); }

void unregisterClientLoginEventHooks() { ClientLoginEventHook::unhook(); }
} // namespace CauldronZero::event
