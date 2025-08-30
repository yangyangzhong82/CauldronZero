#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Player;
class ItemStack;

namespace CauldronZero::event {

// --- PlayerCompleteUsingItemBeforeEvent ---
// 此事件在玩家完成使用物品之前触发。
// 它是可取消的，用以阻止该行为。
class PlayerCompleteUsingItemBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&    mPlayer;
    ItemStack const& mItem;

public:
    constexpr explicit PlayerCompleteUsingItemBeforeEvent(Player& player, ItemStack const& item)
    : mPlayer(player),
      mItem(item) {}

    CZ_API Player&          getPlayer() const;
    CZ_API ItemStack const& getItem() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- PlayerCompleteUsingItemAfterEvent ---
// 此事件在玩家完成使用物品之后触发。
class PlayerCompleteUsingItemAfterEvent final : public ll::event::Event {
protected:
    Player&    mPlayer;
    ItemStack const& mItem;

public:
    constexpr explicit PlayerCompleteUsingItemAfterEvent(Player& player, ItemStack const& item)
    : mPlayer(player),
      mItem(item) {}

    CZ_API Player&          getPlayer() const;
    CZ_API ItemStack const& getItem() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerPlayerCompleteUsingItemEventHooks();
void unregisterPlayerCompleteUsingItemEventHooks();

} // namespace CauldronZero::event
