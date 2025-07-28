#pragma once

#include "CauldronZero/Macros.h"
#include "mc/network/ConnectionRequest.h"
#include "mc/network/NetworkIdentifier.h"
#include <ll/api/event/Event.h>
#include <string>

namespace CauldronZero::event {

class ClientLoginBeforeEvent final : public ll::event::Event {
protected:
    ConnectionRequest&       mConnectionRequest;
    const std::string        mPlayerName;
    const NetworkIdentifier& mNetworkIdentifier;

public:
    constexpr explicit ClientLoginBeforeEvent(
        ConnectionRequest&       connReq,
        const std::string&       name,
        const NetworkIdentifier& netId
    )
    : mConnectionRequest(connReq),
      mPlayerName(name),
      mNetworkIdentifier(netId) {}

    CZ_API ConnectionRequest& getConnectionRequest() const;
    CZ_API const std::string& getPlayerName() const;

    // API to disconnect the player
    CZ_API void disconnect(const std::string& message = "Connection rejected by server.") const;
};
void registerClientLoginEventHooks();

void unregisterClientLoginEventHooks();
} // namespace CauldronZero::event
