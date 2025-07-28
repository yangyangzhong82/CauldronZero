#pragma once

#include "CauldronZero/Macros.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/BlockPos.h"
#include <ll/api/event/EventRefObjSerializer.h>

class LevelContainerModel;
class ItemStack;

namespace CauldronZero::event {

class ContainerItemChangeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    LevelContainerModel& mContainerModel;
    int                  mSlot;
    const ItemStack&     mOldItem;
    const ItemStack&     mNewItem;
    const BlockPos&      mBlockPos;
    int                  mRealSlot;

public:
    constexpr explicit ContainerItemChangeEvent(
        LevelContainerModel& containerModel,
        int                  slot,
        const ItemStack&     oldItem,
        const ItemStack&     newItem,
        const BlockPos&      blockPos,
        int                  realSlot
    )
    : mContainerModel(containerModel),
      mSlot(slot),
      mOldItem(oldItem),
      mNewItem(newItem),
      mBlockPos(blockPos),
      mRealSlot(realSlot) {}

    CZ_API LevelContainerModel& getContainerModel() const;
    CZ_API int                  getSlot() const;
    CZ_API const ItemStack&     getOldItem() const;
    CZ_API const ItemStack&     getNewItem() const;
    CZ_API const BlockPos&      getBlockPos() const;
    CZ_API int                  getRealSlot() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerContainerItemChangeEventHooks();
void unregisterContainerItemChangeEventHooks();

} // namespace CauldronZero::event
