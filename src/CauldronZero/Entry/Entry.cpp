#include "Entry.h"
#include "CauldronZero/Global.h"
#include "CauldronZero/Utils/PapiPlayerExpansion.h"
#include "CauldronZero/events/world/ContainerItemChangeEvent.h"
#include "CauldronZero/events/world/ExplosionEvent.h"
#include "CauldronZero/events/world/actor/ActorChangeDimensionEvent.h" // Added for ActorChangeDimensionHook
#include "CauldronZero/events/world/actor/ActorDestroyBlockEvent.h"
#include "CauldronZero/events/world/actor/ActorRideEvent.h"
#include "CauldronZero/events/world/actor/MobHurtEffectEvent.h"
#include "CauldronZero/events/world/actor/MobTotemRespawnEvent.h"
#include "CauldronZero/events/world/actor/ProjectileHitEvent.h"
#include "CauldronZero/events/world/actor/WitherDestroyEvent.h"
#include "CauldronZero/events/world/actor/ActorPressurePlateTriggerEvent.h" // Added for ActorPressurePlateTriggerEvent
#include "CauldronZero/events/world/actor/ActorEffectUpdateEvent.h"
#include "CauldronZero/events/world/actor/MobHealthChangeEvent.h" // Added for MobHealthChangeEvent
#include "CauldronZero/events/world/actor/player/PlayerChangeDimensionEvent.h" // Added for PlayerChangeDimensionHook
#include "CauldronZero/events/world/actor/player/PlayerDropItemEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerEditSignEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerInteractEntityEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerCompleteUsingItemEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerArmorStandSwapItemEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerAttackBlockEvent.h"
#include "CauldronZero/events/world/actor/ProjectileCreateEvent.h"
#include "CauldronZero/events/server/ClientLoginEvent.h"
#include "CauldronZero/events/world/block/BlockFallEvent.h" // Added for BlockFallEvent
#include "CauldronZero/events/world/block/DragonEggBlockEvent.h"
#include "CauldronZero/events/world/block/FarmTrampleEvent.h"
#include "CauldronZero/events/world/block/ItemFrameBlockEvent.h"
#include "CauldronZero/events/world/block/LiquidBlockSpread.h"
#include "CauldronZero/events/world/block/LiquidBlocktrySpread.h" // Added for LiquidBlockTrySpreadEvent
#include "CauldronZero/events/world/block/RedstoneUpdateEvent.h" // Added for RedstoneUpdateEvent
#include "CauldronZero/events/world/WeatherUpdateEvent.h" // Added for WeatherUpdateEvent
#include "CauldronZero/events/world/block/FireBlockBurnEvent.h"
#include "CauldronZero/events/world/block/MossGrowthEvent.h" // Added for MossGrowthEvent
#include "CauldronZero/events/world/block/PistonPushEvent.h" // Added for PistonPushEvent
#include "CauldronZero/logger.h"
#include "CauldronZero/test/event.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/Config.h"
#include <string>

namespace CauldronZero {
} // namespace CauldronZero

static ll::io::LogLevel logLevelFromString(const std::string& level) {
    if (level == "Off") return ll::io::LogLevel::Off;
    if (level == "Fatal") return ll::io::LogLevel::Fatal;
    if (level == "Error") return ll::io::LogLevel::Error;
    if (level == "Warn") return ll::io::LogLevel::Warn;
    if (level == "Info") return ll::io::LogLevel::Info;
    if (level == "Debug") return ll::io::LogLevel::Debug;
    if (level == "Trace") return ll::io::LogLevel::Trace;
    return ll::io::LogLevel::Debug; // 默认值
}

namespace CauldronZero {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}


bool Entry::load() {

    getSelf().getLogger().debug("Loading...");
    config.emplace();
    if (!ll::config::loadConfig(*config, getSelf().getConfigDir() / u8"config.json")) {
        ll::config::saveConfig(*config, getSelf().getConfigDir() / u8"config.json");
    }
    getSelf().getLogger().setLevel(logLevelFromString(config->logLevel));

    CauldronZero::Utils::Papi::registerPlayerExpansion();

    return true;
}

bool Entry::enable() {
    getSelf().getLogger().debug("Enabling...");
     if (config->features.item_frame_attack_hook_enabled) {
        event::registerFrameBlockattackEventHooks();
    }
    if (config->features.item_frame_use_hook_enabled) {
        event::registerFrameBlockuseEventHooks();
    }
    if (config->features.actor_change_dimension_hook_enabled) {
        event::registerActorChangeDimensionEventHooks();
    }
    if (config->features.player_change_dimension_hook_enabled) {
        event::registerPlayerChangeDimensionHooks();
    }
    if (config->features.player_drop_item_hook_enabled) {
        event::registerPlayerDropItemHooks();
    }
    if (config->features.player_interact_entity_hook_enabled) {
        event::registerPlayerInteractEntityHooks();
    }
    if (config->features.explosion_hook_enabled) {
        event::registerExplosionEventHooks();
    }
    if (config->features.player_edit_sign_hook_enabled) {
        event::registerPlayerEditSignHooks();
    }
    if (config->features.farm_trample_hook_enabled) {
        event::registerFarmTrampleEventHooks();
    }
    if (config->features.wither_destroy_hook_enabled) {
        event::registerWitherDestroyEventHooks();
    }
    if (config->features.liquid_spread_hook_enabled) {
        event::registerLiquidSpreadEventHooks();
    }
    if (config->features.dragon_egg_teleport_hook_enabled) {
        event::registerDragonEggBlockEventHooks();
    }
    if (config->features.mob_hurt_effect_hook_enabled) {
        event::registerMobHurtEffectEventHooks();
    }
    if (config->features.projectile_hit_hook_enabled) {
        event::registerProjectileHitEventHooks();
    }
    if (config->features.actor_ride_hook_enabled) {
        event::registerActorRideEventHooks();
    }
    if (config->features.container_item_change_hook_enabled) {
        event::registerContainerItemChangeEventHooks();
    }
    if (config->features.actor_destroy_block_hook_enabled) {
        event::registerActorDestroyBlockHooks();
    }
    if (config->features.mob_totem_respawn_hook_enabled) {
        event::registerMobTotemRespawnEventHooks();
    }
    if (config->features.client_login_hook_enabled) {
        event::registerClientLoginEventHooks();
    }
    if (config->features.player_armor_stand_swap_item_hook_enabled) {
        event::registerPlayerArmorStandSwapItemHooks();
    }
    if (config->features.projectile_create_hook_enabled) {
        event::registerProjectileCreateHooks();
    }
    if (config->features.enable_start_destroy_block_hook_enabled) {
        event::registerPlayerAttackBlockHooks();
    }
    if (config->features.actor_pressure_plate_trigger_hook_enabled) {
        event::registerActorPressurePlateTriggerEventHooks();
    }
    if (config->features.actor_effect_update_hook_enabled) {
        event::registerActorEffectUpdateEventHooks();
    }
    if (config->features.mob_health_change_hook_enabled) { // Added for MobHealthChangeEvent
        event::registerMobHealthChangeEventHooks();
    }
    if (config->features.player_complete_using_item_hook_enabled) {
        event::registerPlayerCompleteUsingItemEventHooks();
    }
    if (config->features.weather_update_hook_enabled) {
        event::registerWeatherUpdateEventHooks();
    }
    if (config->features.fire_burn_hook_enabled) {
        event::registerFireBlockEventHooks();
    }
    if (config->features.block_fall_hook_enabled) { // Added for BlockFallEvent
        event::registerBlockFallEventHooks();
    }
    if (config->features.redstone_update_hook_enabled) {
        event::registerRedstoneUpdateEventHooks();
    }
    if (config->features.moss_growth_hook_enabled) {
        event::registerMossGrowthEventHooks();
    }
    if (config->features.piston_push_hook_enabled) {
        event::registerPistonPushEventHooks();
    }
    if (config->features.liquid_try_spread_hook_enabled) {
        event::registerLiquidTrySpreadEventHooks();
    }

event::registerTestEventListeners();
    return true;
}

bool Entry::disable() {
    getSelf().getLogger().debug("Disabling...");

    if (config->features.item_frame_attack_hook_enabled) {
        event::unregisterFrameBlockattackEventHooks();
    }
    if (config->features.item_frame_use_hook_enabled) {
        event::unregisterFrameBlockuseEventHooks();
    }
    if (config->features.actor_change_dimension_hook_enabled) {
        event::unregisterActorChangeDimensionEventHooks();
    }
    if (config->features.player_change_dimension_hook_enabled) {
        event::unregisterPlayerChangeDimensionHooks();
    }
    if (config->features.player_drop_item_hook_enabled) {
        event::unregisterPlayerDropItemHooks();
    }
    if (config->features.player_interact_entity_hook_enabled) {
        event::unregisterPlayerInteractEntityHooks();
    }
    if (config->features.explosion_hook_enabled) {
        event::unregisterExplosionEventHooks();
    }
    if (config->features.player_edit_sign_hook_enabled) {
        event::unregisterPlayerEditSignHooks();
    }
    if (config->features.farm_trample_hook_enabled) {
        event::unregisterFarmTrampleEventHooks();
    }
    if (config->features.wither_destroy_hook_enabled) {
        event::unregisterWitherDestroyEventHooks();
    }
    if (config->features.liquid_spread_hook_enabled) {
        event::unregisterLiquidSpreadEventHooks();
    }
    if (config->features.dragon_egg_teleport_hook_enabled) {
        event::unregisterDragonEggBlockEventHooks();
    }
    if (config->features.mob_hurt_effect_hook_enabled) {
        event::unregisterMobHurtEffectEventHooks();
    }
    if (config->features.projectile_hit_hook_enabled) {
        event::unregisterProjectileHitEventHooks();
    }
    if (config->features.actor_ride_hook_enabled) {
        event::unregisterActorRideEventHooks();
    }
    if (config->features.container_item_change_hook_enabled) {
        event::unregisterContainerItemChangeEventHooks();
    }
    if (config->features.actor_destroy_block_hook_enabled) {
        event::unregisterActorDestroyBlockHooks();
    }
    if (config->features.mob_totem_respawn_hook_enabled) {
        event::unregisterMobTotemRespawnEventHooks();
    }
    if (config->features.client_login_hook_enabled) {
        event::unregisterClientLoginEventHooks();
    }
    if (config->features.player_armor_stand_swap_item_hook_enabled) {
        event::unregisterPlayerArmorStandSwapItemHooks();
    }
    if (config->features.projectile_create_hook_enabled) {
        event::unregisterProjectileCreateHooks();
    }
    if (config->features.enable_start_destroy_block_hook_enabled) {
        event::unregisterPlayerAttackBlockHooks();
    }
    if (config->features.actor_pressure_plate_trigger_hook_enabled) {
        event::unregisterActorPressurePlateTriggerEventHooks();
    }
    if (config->features.actor_effect_update_hook_enabled) {
        event::unregisterActorEffectUpdateEventHooks();
    }
    if (config->features.mob_health_change_hook_enabled) { // Added for MobHealthChangeEvent
        event::unregisterMobHealthChangeEventHooks();
    }
    if (config->features.player_complete_using_item_hook_enabled) {
        event::unregisterPlayerCompleteUsingItemEventHooks();
    }
    if (config->features.weather_update_hook_enabled) {
        event::unregisterWeatherUpdateEventHooks();
    }
    if (config->features.fire_burn_hook_enabled) {
        event::unregisterFireBlockEventHooks();
    }
    if (config->features.block_fall_hook_enabled) { // Added for BlockFallEvent
        event::unregisterBlockFallEventHooks();
    }
    if (config->features.redstone_update_hook_enabled) {
        event::unregisterRedstoneUpdateEventHooks();
    }
    if (config->features.moss_growth_hook_enabled) {
        event::unregisterMossGrowthEventHooks();
    }
    if (config->features.piston_push_hook_enabled) {
        event::unregisterPistonPushEventHooks();
    }
    if (config->features.liquid_try_spread_hook_enabled) {
        event::unregisterLiquidTrySpreadEventHooks();
    }
    return true;
}

Entry::Entry() : mSelf(*ll::mod::NativeMod::current()) {}

Entry::~Entry() = default;

ll::mod::NativeMod& Entry::getSelf() const { return mSelf; }

} // namespace CauldronZero



LL_REGISTER_MOD(CauldronZero::Entry, CauldronZero::Entry::getInstance());
