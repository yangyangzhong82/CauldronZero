#pragma once
#include "ll/api/Config.h"
struct Config {
    int version = 1;
    std::string logLevel = "Debug";

    struct Features {
        bool item_frame_attack_hook_enabled = true;
        bool item_frame_use_hook_enabled = true;
        bool actor_change_dimension_hook_enabled = true;
        bool player_change_dimension_hook_enabled = true;
        bool player_drop_item_hook_enabled = true;
        bool player_interact_entity_hook_enabled = true;
        bool explosion_hook_enabled = true;
        bool player_edit_sign_hook_enabled = true;
        bool farm_trample_hook_enabled = true;
        bool wither_destroy_hook_enabled = true;
        bool liquid_spread_hook_enabled = true;
        bool dragon_egg_teleport_hook_enabled = true;
        bool mob_hurt_effect_hook_enabled = true;
        bool projectile_hit_hook_enabled = true;
        bool actor_ride_hook_enabled = true;
        bool container_item_change_hook_enabled = true;
        bool actor_destroy_block_hook_enabled = true;
        bool mob_totem_respawn_hook_enabled = true;
        bool client_login_hook_enabled = true;
        bool player_armor_stand_swap_item_hook_enabled = true;
        bool projectile_create_hook_enabled = true;
        bool enable_start_destroy_block_hook_enabled = true;
        bool actor_pressure_plate_trigger_hook_enabled = true;
        bool actor_effect_update_hook_enabled = true;
        bool player_complete_using_item_hook_enabled = true;
        bool weather_update_hook_enabled = true; // Added for WeatherUpdateEvent
        bool fire_burn_hook_enabled = true;
        bool block_fall_hook_enabled = true; // Added for BlockFallEvent
        bool redstone_update_hook_enabled = true;
    } features;
};
