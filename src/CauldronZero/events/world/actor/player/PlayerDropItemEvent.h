#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/item/ItemStack.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Player;

namespace CauldronZero::event {

class PlayerDropItemBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&   mPlayer;
    ItemStack mItem;

public:
    explicit PlayerDropItemBeforeEvent(Player& player, const ItemStack& item) : mPlayer(player), mItem(item) {}

    CZ_API Player& getPlayer() const;
    CZ_API ItemStack& getItem();
    CZ_API const ItemStack& getItem() const;

    void serialize(CompoundTag& nbt) const override;
};

class PlayerDropItemAfterEvent final : public ll::event::Event {
protected:
    Player&   mPlayer;
    ItemStack mItem;

public:
    explicit PlayerDropItemAfterEvent(Player& player, const ItemStack& item) : mPlayer(player), mItem(item) {}

    CZ_API Player& getPlayer() const;
    CZ_API const ItemStack& getItem() const;

    void serialize(CompoundTag& nbt) const override;
};

void registerPlayerDropItemHooks();
void unregisterPlayerDropItemHooks();

} // namespace CauldronZero::event
