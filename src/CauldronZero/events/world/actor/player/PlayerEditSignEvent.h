#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/block/actor/BlockActor.h"
#include "mc/world/level/block/actor/SignBlockActor.h"
#include <ll/api/event/Event.h>
#include <string>

class Player;

namespace CauldronZero::event {

class PlayerEditSignEvent final : public ll::event::Event {
protected:
    Player& mPlayer;
    SignBlockActor::Text& mSignText;
    std::string mOriginalText;

public:
    constexpr explicit PlayerEditSignEvent(Player& player, SignBlockActor::Text& signText)
    : mPlayer(player),
      mSignText(signText),
      mOriginalText(signText.getMessage()) {}

    CZ_API Player& getPlayer() const;
    CZ_API const std::string& getOriginalText() const;
    CZ_API std::string getText() const;
    CZ_API void setText(const std::string& text);

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerPlayerEditSignHooks();
void unregisterPlayerEditSignHooks();

} // namespace CauldronZero::event
