#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/block/actor/SignBlockActor.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <string>

class ServerPlayer;

namespace CauldronZero::event {

class PlayerEditSignBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    ServerPlayer&   mPlayer;
    SignBlockActor& mSign;
    std::string     mNewFrontText;
    std::string     mNewBackText;

public:
    constexpr explicit PlayerEditSignBeforeEvent(
        ServerPlayer&   player,
        SignBlockActor& sign,
        std::string     newFrontText,
        std::string     newBackText
    )
    : mPlayer(player),
      mSign(sign),
      mNewFrontText(std::move(newFrontText)),
      mNewBackText(std::move(newBackText)) {}

    CZ_API ServerPlayer&   getPlayer() const;
    CZ_API SignBlockActor& getSign() const;
    CZ_API std::string& getNewFrontText();
    CZ_API std::string& getNewBackText();

    void serialize(CompoundTag& nbt) const override;
};

class PlayerEditSignAfterEvent final : public ll::event::Event {
protected:
    ServerPlayer&   mPlayer;
    SignBlockActor& mSign;
    std::string     mOldFrontText;
    std::string     mOldBackText;

public:
    constexpr explicit PlayerEditSignAfterEvent(
        ServerPlayer&   player,
        SignBlockActor& sign,
        std::string     oldFrontText,
        std::string     oldBackText
    )
    : mPlayer(player),
      mSign(sign),
      mOldFrontText(std::move(oldFrontText)),
      mOldBackText(std::move(oldBackText)) {}

    CZ_API ServerPlayer&   getPlayer() const;
    CZ_API SignBlockActor& getSign() const;
    CZ_API const std::string& getOldFrontText() const;
    CZ_API const std::string& getOldBackText() const;

    void serialize(CompoundTag& nbt) const override;
};

void registerPlayerEditSignHooks();
void unregisterPlayerEditSignHooks();

} // namespace CauldronZero::event
