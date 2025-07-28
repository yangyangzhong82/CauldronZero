#include "Entry.h"
#include "CauldronZero/Global.h"
#include "CauldronZero/events/world/block/FireBlockEvent.h"
#include "CauldronZero/events/world/block/ItemFrameBlockEvent.h"
#include "CauldronZero/events/world/actor/ActorChangeDimensionEvent.h" // Added for ActorChangeDimensionHook
#include "CauldronZero/events/world/actor/ActorDestroyBlockEvent.h"
#include "CauldronZero/events/world/actor/ActorRideEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerChangeDimensionEvent.h" // Added for PlayerChangeDimensionHook
#include "CauldronZero/events/world/actor/player/PlayerDropItemEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerEditSignEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerInteractEntityEvent.h"
#include "CauldronZero/events/world/actor/MobHurtEffectEvent.h"
#include "CauldronZero/events/world/actor/ProjectileHitEvent.h"
#include "CauldronZero/events/world/actor/WitherDestroyEvent.h"
#include "CauldronZero/events/world/actor/MobTotemRespawnEvent.h"
#include "CauldronZero/events/world/block/FarmTrampleEvent.h"
#include "CauldronZero/events/world/block/LiquidBlockSpread.h"
#include "CauldronZero/events/world/block/DragonEggBlockEvent.h"
#include "CauldronZero/events/world/ContainerItemChangeEvent.h"
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
    if (Global::getInstance().getConfig().get<bool>("features.player_drop_item_hook.enabled", true)) {
        event::registerPlayerDropItemHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.player_interact_entity_hook.enabled", true)) {
        event::registerPlayerInteractEntityHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.explosion_hook.enabled", true)) {
        event::registerExplosionEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.player_edit_sign_hook.enabled", true)) {
        event::registerPlayerEditSignHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.farm_trample_hook.enabled", true)) {
        event::registerFarmTrampleEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.wither_destroy_hook.enabled", true)) {
        event::registerWitherDestroyEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.liquid_spread_hook.enabled", true)) {
        event::registerLiquidSpreadEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.dragon_egg_teleport_hook.enabled", true)) {
        event::registerDragonEggBlockEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.mob_hurt_effect_hook.enabled", true)) {
        event::registerMobHurtEffectEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.projectile_hit_hook.enabled", true)) {
        event::registerProjectileHitEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.actor_ride_hook.enabled", true)) {
        event::registerActorRideEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.container_item_change_hook.enabled", true)) {
        event::registerContainerItemChangeEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.actor_destroy_block_hook.enabled", true)) {
        event::registerActorDestroyBlockHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.mob_totem_respawn_hook.enabled", true)) {
        event::registerMobTotemRespawnEventHooks();
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
    if (Global::getInstance().getConfig().get<bool>("features.player_drop_item_hook.enabled", true)) {
        event::unregisterPlayerDropItemHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.player_interact_entity_hook.enabled", true)) {
        event::unregisterPlayerInteractEntityHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.explosion_hook.enabled", true)) {
        event::unregisterExplosionEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.player_edit_sign_hook.enabled", true)) {
        event::unregisterPlayerEditSignHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.farm_trample_hook.enabled", true)) {
        event::unregisterFarmTrampleEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.wither_destroy_hook.enabled", true)) {
        event::unregisterWitherDestroyEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.liquid_spread_hook.enabled", true)) {
        event::unregisterLiquidSpreadEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.dragon_egg_teleport_hook.enabled", true)) {
        event::unregisterDragonEggBlockEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.mob_hurt_effect_hook.enabled", true)) {
        event::unregisterMobHurtEffectEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.projectile_hit_hook.enabled", true)) {
        event::unregisterProjectileHitEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.actor_ride_hook.enabled", true)) {
        event::unregisterActorRideEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.container_item_change_hook.enabled", true)) {
        event::unregisterContainerItemChangeEventHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.actor_destroy_block_hook.enabled", true)) {
        event::unregisterActorDestroyBlockHooks();
    }
    if (Global::getInstance().getConfig().get<bool>("features.mob_totem_respawn_hook.enabled", true)) {
        event::unregisterMobTotemRespawnEventHooks();
    }
    return true;
}

} // namespace CauldronZero

LL_REGISTER_MOD(CauldronZero::Entry, CauldronZero::Entry::getInstance());
