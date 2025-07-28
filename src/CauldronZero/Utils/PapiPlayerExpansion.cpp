#include "PapiPlayerExpansion.h"
#include "PlaceholderManager.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include <any>
#include <string>

namespace CauldronZero::Utils::Papi {

// Helper function to safely get the player from the context
inline const Player* getPlayerFromContext(const std::any& context) {
    if (!context.has_value() || context.type() != typeid(Player*)) {
        return nullptr;
    }
    return std::any_cast<Player*>(context);
}

void registerPlayerExpansion() {
    auto& manager = PlaceholderManager::getInstance();

    auto registerPlayerPlaceholder = [&](const std::string& key, std::function<std::string(const Player*)> callback) {
        manager.registerPlaceholder("player_" + key, [key, callback](const std::any& context) -> std::string {
            if (const auto* player = getPlayerFromContext(context)) {
                if (player) {
                    return callback(player);
                }
                return "{player_is_null}";
            }
            return "{invalid_context_for_player_" + key + "}";
        });
    };

    // Player's Real Name
    registerPlayerPlaceholder("name", [](const Player* player) { return player->getRealName(); });

    // Player's XUID
    registerPlayerPlaceholder("xuid", [](const Player* player) { return player->getXuid(); });

    // Player's UUID
    registerPlayerPlaceholder("uuid", [](const Player* player) { return player->getUuid().asString(); });

    // Player's Position X
    registerPlayerPlaceholder("pos_x", [](const Player* player) { return std::to_string(player->getPosition().x); });

    // Player's Position Y
    registerPlayerPlaceholder("pos_y", [](const Player* player) { return std::to_string(player->getPosition().y); });

    // Player's Position Z
    registerPlayerPlaceholder("pos_z", [](const Player* player) { return std::to_string(player->getPosition().z); });

    // Player's Max Health
    registerPlayerPlaceholder("max_health", [](const Player* player) { return std::to_string(player->getMaxHealth()); });

}

} // namespace CauldronZero::Utils::Papi