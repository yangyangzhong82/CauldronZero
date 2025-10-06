#pragma once

#include "CauldronZero/Macros.h"
#include "mc/network/ConnectionRequest.h"
#include "mc/network/NetworkIdentifier.h"
#include "mc/network/packet/LoginPacket.h"
#include "mc/platform/UUID.h"
#include <ll/api/event/Event.h>
#include <string>

namespace CauldronZero::event {

class ClientLoginBeforeEvent final : public ll::event::Event {
protected:
    ConnectionRequest&       mConnectionRequest;
    const std::string        mPlayerName;
    const NetworkIdentifier& mNetworkIdentifier;
    std::shared_ptr<LoginPacket> mLoginPacket;
public:
    explicit ClientLoginBeforeEvent(
        ConnectionRequest&       connReq,
        const std::string&       name,
        const NetworkIdentifier& netId,
        std::shared_ptr<LoginPacket> loginPacket
    )
    : mConnectionRequest(connReq),
      mPlayerName(name),
      mNetworkIdentifier(netId),
      mLoginPacket(loginPacket) {}

    CZ_API ConnectionRequest& getConnectionRequest() const;
    CZ_API const std::string& getPlayerName() const;
    CZ_API const NetworkIdentifier& getNetworkIdentifier() const;
    CZ_API std::shared_ptr<LoginPacket> getLoginPacket() const;

    CZ_API std::string getIp() const;
    CZ_API std::string getXuid() const;
    CZ_API mce::UUID   getUuid() const;
    CZ_API std::string getDeviceId() const;

    // API to disconnect the player
    CZ_API void disconnect(const std::string& message = "Connection rejected by server.") const;
};
void registerClientLoginEventHooks();

void unregisterClientLoginEventHooks();
} // namespace CauldronZero::event
