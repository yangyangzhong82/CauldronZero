#include "PlayerEditSignEvent.h"
#include "CauldronZero/mc/nbt/NbtUtils.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/service/Bedrock.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/block/actor/SignBlockActor.h"



namespace CauldronZero::event {

Player& PlayerEditSignEvent::getPlayer() const { return mPlayer; }

const std::string& PlayerEditSignEvent::getOriginalText() const { return mOriginalText; }

std::string PlayerEditSignEvent::getText() const { return mSignText.getMessage(); }

void PlayerEditSignEvent::setText(const std::string& text) { mSignText.setMessage(text); }

void PlayerEditSignEvent::serialize(CompoundTag& nbt) const {
    nbt["player"]       = ll::event::serializeRefObj(mPlayer);
    nbt["originalText"] = mOriginalText;
    nbt["text"]         = mSignText.getMessage();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    SignBlockActorSetEditedByHook,
    ll::memory::HookPriority::Normal,
    SignBlockActor::Text,
    &SignBlockActor::Text::setEditedBy,
    void,
    std::string const& playerXuid
) {
    auto* player = ll::service::getLevel()->getPlayerByXuid(playerXuid);
    if (player) {
        auto event = PlayerEditSignEvent(*player, *this);
        ll::event::EventBus::getInstance().publish(event);
    }
    origin(playerXuid);
}




void registerPlayerEditSignHooks() {
    SignBlockActorSetEditedByHook::hook();

}

void unregisterPlayerEditSignHooks() {
    SignBlockActorSetEditedByHook::unhook();
}

} // namespace CauldronZero::event
