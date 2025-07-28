#include "ContainerItemChangeEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/containers/models/LevelContainerModel.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/level/BlockPos.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <memory>

namespace CauldronZero::event {

LevelContainerModel& ContainerItemChangeEvent::getContainerModel() const { return mContainerModel; }
int                  ContainerItemChangeEvent::getSlot() const { return mSlot; }
const ItemStack&     ContainerItemChangeEvent::getOldItem() const { return mOldItem; }
const ItemStack&     ContainerItemChangeEvent::getNewItem() const { return mNewItem; }
const BlockPos&      ContainerItemChangeEvent::getBlockPos() const { return mBlockPos; }
int                  ContainerItemChangeEvent::getRealSlot() const { return mRealSlot; }

void ContainerItemChangeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["container"] = ll::event::serializeRefObj(mContainerModel);
    nbt["slot"]      = mSlot;
    nbt["old_item"]  = ll::event::serializeRefObj(mOldItem);
    nbt["new_item"]  = ll::event::serializeRefObj(mNewItem);
    nbt["x"] = mBlockPos.x;
    nbt["y"] = mBlockPos.y;
    nbt["z"] = mBlockPos.z;
    nbt["real_slot"] = mRealSlot;
}

class ContainerItemChangeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ContainerItemChangeEvent> {};


LL_TYPE_INSTANCE_HOOK(
    ContainerChangeHook,
    HookPriority::Normal,
    LevelContainerModel,
    &LevelContainerModel::$_onItemChanged,
    void,
    int              slot,
    ItemStack const& oldItem,
    ItemStack const& newItem
) {
    if (*reinterpret_cast<void***>(this) != LevelContainerModel::$vftable()) {
        return origin(slot, oldItem, newItem);
    }

    try {
        auto& containerModel = *this;
        auto& blockPos       = containerModel.mBlockPos;
        int   realSlot       = slot + containerModel._getContainerOffset();
        auto  event          = CauldronZero::event::ContainerItemChangeEvent(containerModel, slot, oldItem, newItem, blockPos, realSlot);
        
        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return;
        }

        return origin(slot, oldItem, newItem);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "ContainerChangeHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(slot, oldItem, newItem);
    } catch (const std::exception& e) {
        logger.warn("ContainerChangeHook 发生 C++ 异常: {}", e.what());
        return origin(slot, oldItem, newItem);
    } catch (...) {
        logger.warn("ContainerChangeHook 发生未知异常！");
        return origin(slot, oldItem, newItem);
    }
}

void registerContainerItemChangeEventHooks() { ContainerChangeHook::hook(); }

void unregisterContainerItemChangeEventHooks() { ContainerChangeHook::unhook(); }

} // namespace CauldronZero::event
