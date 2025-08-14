#pragma once

#include "CauldronZero/Macros.h"
#include "mc/deps/shared_types/legacy/item/EquipmentSlot.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Player;
class ArmorStand;

namespace CauldronZero::event {

class PlayerArmorStandSwapItemBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&                              mPlayer;
    ArmorStand&                          mArmorStand;
    ::SharedTypes::Legacy::EquipmentSlot mSlot;

public:
    constexpr explicit PlayerArmorStandSwapItemBeforeEvent(
        Player&                              player,
        ArmorStand&                          armorStand,
        ::SharedTypes::Legacy::EquipmentSlot slot
    )
    : mPlayer(player),
      mArmorStand(armorStand),
      mSlot(slot) {}

    CZ_API Player&                              getPlayer() const;
    CZ_API ArmorStand&                          getArmorStand() const;
    CZ_API ::SharedTypes::Legacy::EquipmentSlot getSlot() const;

    void serialize(CompoundTag& nbt) const override;
};

class PlayerArmorStandSwapItemAfterEvent final : public ll::event::Event {
protected:
    Player&                              mPlayer;
    ArmorStand&                          mArmorStand;
    ::SharedTypes::Legacy::EquipmentSlot mSlot;

public:
    constexpr explicit PlayerArmorStandSwapItemAfterEvent(
        Player&                              player,
        ArmorStand&                          armorStand,
        ::SharedTypes::Legacy::EquipmentSlot slot
    )
    : mPlayer(player),
      mArmorStand(armorStand),
      mSlot(slot) {}

    CZ_API Player&                              getPlayer() const;
    CZ_API ArmorStand&                          getArmorStand() const;
    CZ_API ::SharedTypes::Legacy::EquipmentSlot getSlot() const;

    void serialize(CompoundTag& nbt) const override;
};

void registerPlayerArmorStandSwapItemHooks();
void unregisterPlayerArmorStandSwapItemHooks();

} // namespace CauldronZero::event