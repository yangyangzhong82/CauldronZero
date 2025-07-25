#include "Entry.h"
#include "CauldronZero/Global.h"
#include "CauldronZero/events/world/block/FireBlockEvent.h"
#include "CauldronZero/events/world/block/ItemFrameBlockEvent.h"
#include "CauldronZero/events/world/actor/ActorChangeDimensionEvent.h" // Added for ActorChangeDimensionHook
#include "CauldronZero/events/world/actor/player/PlayerChangeDimensionEvent.h" // Added for PlayerChangeDimensionHook
#include "CauldronZero/events/world/ExplosionEvent.h"
#include "CauldronZero/logger.h"
#include "ll/api/mod/RegisterHelper.h"
#include "CauldronZero/test/event.h"
namespace CauldronZero {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

Entry::Entry() : mSelf(*ll::mod::NativeMod::current()) {}

Entry::~Entry() = default;

ll::mod::NativeMod& Entry::getSelf() const { return mSelf; }

bool Entry::load() {
    getSelf().getLogger().debug("Loading...");
    Global::getInstance().load();
    return true;
}

bool Entry::enable() {
    getSelf().getLogger().debug("Enabling...");
    if (Global::getInstance().getConfig().get<bool>("features.fire_spread_control.enabled", true)) {
        event::registerFireBlockEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.item_frame_attack_hook.enabled", true)) {
        event::registerFrameBlockattackEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.item_frame_use_hook.enabled", true)) {
        event::registerFrameBlockuseEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.actor_change_dimension_hook.enabled", true)) {
        event::registerActorChangeDimensionEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.player_change_dimension_hook.enabled", true)) {
        event::registerPlayerChangeDimensionHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.explosion_hook.enabled", true)) {
        event::registerExplosionEventHooks();
    }
    event::registerTestEventListeners();
    return true;
}

bool Entry::disable() {
    getSelf().getLogger().debug("Disabling...");
    if (Global::getInstance().getConfig().get<bool>("features.fire_spread_control.enabled", true)) {
        event::unregisterFireBlockEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.item_frame_attack_hook.enabled", true)) {
        event::unregisterFrameBlockattackEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.item_frame_use_hook.enabled", true)) {
        event::unregisterFrameBlockuseEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.actor_change_dimension_hook.enabled", true)) {
        event::unregisterActorChangeDimensionEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.player_change_dimension_hook.enabled", true)) {
        event::unregisterPlayerChangeDimensionHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.explosion_hook.enabled", true)) {
        event::unregisterExplosionEventHooks();
    }
    return true;
}

} // namespace CauldronZero

LL_REGISTER_MOD(CauldronZero::Entry, CauldronZero::Entry::getInstance());
