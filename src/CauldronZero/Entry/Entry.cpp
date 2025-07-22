#include "CauldronZero/Entry/Entry.h"
#include "CauldronZero/Utils/LogLevelHelper.h"
#include "CauldronZero/logger.h"
#include "ll/api/mod/RegisterHelper.h"

namespace CauldronZero {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

void Entry::setupConfig() {
    auto configFilePath = getSelf().getModDir() / "config.json";

    if (!std::filesystem::exists(configFilePath)) {
        logger.info("Config file not found, creating a default one at: {}", configFilePath.string());
        mConfig.set("log_level", std::string("Info"));
        mConfig.save(configFilePath.string());
    } else {
        if (!mConfig.load(configFilePath.string())) {
            logger.error("Failed to load config file, please check its format.");
        }
    }

    auto updateLogLevel = [this] {
        auto                                                  levelStr = mConfig.get<std::string>("log_level", "Info");
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


bool Entry::load() {
    getSelf().getLogger().debug("Loading...");
    setupConfig();
    // Code for loading the mod goes here.
    return true;
}

bool Entry::enable() {
    getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    return true;
}

bool Entry::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    return true;
}

} // namespace CauldronZero

LL_REGISTER_MOD(CauldronZero::Entry, CauldronZero::Entry::getInstance());
