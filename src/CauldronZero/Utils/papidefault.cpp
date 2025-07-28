#include "mc/world/actor/player/Player.h"
#include "PlaceholderManager.h"
#include <any>

namespace CauldronZero::Utils::Papi {

/**
 * @brief Registers the default set of placeholders.
 *        This function should be called during plugin initialization.
 */
void registerDefaultPlaceholders() {
    auto& manager = CauldronZero::Utils::PlaceholderManager::getInstance();

    // Register a placeholder for the player's real name.
    // The context for this placeholder is expected to be a Player*.
    manager.registerPlaceholder("player_name", [](const std::any& context) -> std::string {
        // Check if the context holds a value and if it's of type Player*
        if (!context.has_value() || context.type() != typeid(Player*)) {
            return "{invalid_context_for_player_name}";
        }

        try {
            // Attempt to cast the context to a Player*
            if (const auto player = std::any_cast<Player*>(context)) {
                // Ensure the player pointer is not null
                if (player) {
                    return player->getRealName();
                }
            }
        } catch (const std::bad_any_cast&) {
            // This catch block is a safeguard, though the type check above should prevent it.
            return "{cast_failed_for_player_name}";
        }

        // Return a default value if the player pointer is null
        return "{player_is_null}";
    });

    // You can register more default placeholders here in the future.
}

} // namespace CauldronZero::Utils::Papi
