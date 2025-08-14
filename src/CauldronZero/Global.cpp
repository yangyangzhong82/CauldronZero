#include "Global.h"
#include "CauldronZero/Entry/Entry.h"
#include "CauldronZero/Utils/LogLevelHelper.h"
#include "CauldronZero/logger.h"
#include <filesystem>

namespace CauldronZero {

Global& Global::getInstance() {
    static Global instance;
    return instance;
}

Global::Global()  = default;
Global::~Global() = default;

Config& Global::getConfig() { return mConfig; }

void Global::setupConfig() {
    auto configFilePath = Entry::getInstance().getSelf().getModDir() / "config.json";

    if (!std::filesystem::exists(configFilePath)) {
        logger.info("Config file not found, creating a default one at: {}", configFilePath.string());
        mConfig.set("log_level", std::string("Info"));
        mConfig.set("features.fire_spread_control.enabled", true);
        mConfig.set("features.fire_spread_control.burn_chance_multiplier", 1.0);
        mConfig.set("features.fire_spread_control.spread_chance_multiplier", 1.0);
        mConfig.set("features.item_frame_attack_hook.enabled", true); 
        mConfig.set("features.item_frame_use_hook.enabled", true);  
        mConfig.set("features.actor_change_dimension_hook.enabled", true);
        mConfig.set("features.player_change_dimension_hook.enabled", true); // 添加新的配置项
        mConfig.set("features.player_drop_item_hook.enabled", true);
        mConfig.set("features.player_interact_entity_hook.enabled", true);
        mConfig.set("features.player_edit_sign_hook.enabled", true);
        mConfig.set("features.explosion_hook.enabled", true);
        mConfig.set("features.farm_trample_hook.enabled", true);
        mConfig.set("features.wither_destroy_hook.enabled", true);
        mConfig.set("features.liquid_spread_hook.enabled", true);
        mConfig.set("features.dragon_egg_teleport_hook.enabled", true);
        mConfig.set("features.mob_hurt_effect_hook.enabled", true);
        mConfig.set("features.projectile_hit_hook.enabled", true);
        mConfig.set("features.actor_ride_hook.enabled", true);
        mConfig.set("features.container_item_change_hook.enabled", true);
        mConfig.set("features.mob_totem_respawn_hook.enabled", true);
        mConfig.set("features.client_login_hook.enabled", true);
        mConfig.set("features.player_armor_stand_swap_item_hook.enabled", true);
        mConfig.set("features.projectile_create_hook.enabled", true);
        mConfig.save(configFilePath.string());
    } else {
        if (!mConfig.load(configFilePath.string())) {
            logger.error("Failed to load config file, please check its format.");
        }
    }

    auto updateLogLevel = [this] {
        auto levelStr = mConfig.get<std::string>("log_level", "Info");
        if (auto it = getLogLevelMap().find(levelStr); it != getLogLevelMap().end()) {
            logger.setLevel(it->second);
            logger.info("Logger level set to {}.", levelStr);
        } else {
            logger.warn("Invalid log level '{}' in config, defaulting to Info.", levelStr);
            logger.setLevel(ll::io::LogLevel::Info);
        }
    };

    updateLogLevel();
    mConfig.watch([this, updateLogLevel] { updateLogLevel(); });
}

bool Global::load() {
    setupConfig();
    return true;
}

} // namespace CauldronZero
