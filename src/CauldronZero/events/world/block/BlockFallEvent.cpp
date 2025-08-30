#include "CauldronZero/events/world/block/BlockFallEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/actor/item/FallingBlockActor.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/FallingBlock.h"

namespace CauldronZero::event {

// --- BlockFallBeforeEvent ---
BlockSource&    BlockFallBeforeEvent::getRegion() const { return mRegion; }
const BlockPos& BlockFallBeforeEvent::getPos() const { return mPos; }
const Block&    BlockFallBeforeEvent::getOldBlock() const { return mOldBlock; }
bool            BlockFallBeforeEvent::isCreative() const { return mCreative; }

void BlockFallBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["dimension"] = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]       = ListTag{mPos.x, mPos.y, mPos.z};

    nbt["creative"] = mCreative;
}

class BlockFallBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, BlockFallBeforeEvent> {};

// --- BlockFallAfterEvent ---
BlockSource&    BlockFallAfterEvent::getRegion() const { return mRegion; }
const BlockPos& BlockFallAfterEvent::getPos() const { return mPos; }
const Block&    BlockFallAfterEvent::getOldBlock() const { return mOldBlock; }
bool            BlockFallAfterEvent::isCreative() const { return mCreative; }

void BlockFallAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["dimension"] = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]       = ListTag{mPos.x, mPos.y, mPos.z};

    nbt["creative"] = mCreative;

}

class BlockFallAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, BlockFallAfterEvent> {};


// --- 挂钩 ---
LL_TYPE_INSTANCE_HOOK(
    BlockFallEventHook,
    ll::memory::HookPriority::Normal,
    FallingBlock,
    &FallingBlock::$startFalling,
    void,
    ::BlockSource&    region,
    ::BlockPos const& pos,
    ::Block const&    oldBlock,
    bool              creative
) {
    try {
        auto beforeEvent = BlockFallBeforeEvent(region, pos, oldBlock, creative);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return; // 取消事件，不让方块下落
        }

        origin(region, pos, oldBlock, creative); 

        auto afterEvent = BlockFallAfterEvent(region, pos, oldBlock, creative);
        ll::event::EventBus::getInstance().publish(afterEvent);

        return;
    } catch (const SEH_Exception& e) {
        logger.error(
            "在 BlockFallEventHook 中捕获到 SEH 异常: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(region, pos, oldBlock, creative);
        throw; // 重新抛出异常
    } catch (const std::exception& e) {
        logger.error("在 BlockFallEventHook 中捕获到标准异常: {}", e.what());
        origin(region, pos, oldBlock, creative);
        throw; // 重新抛出异常
    } catch (...) {
        logger.error("在 BlockFallEventHook 中捕获到未知异常");
        origin(region, pos, oldBlock, creative);
        throw; // 重新抛出异常
    }
}

void registerBlockFallEventHooks() { BlockFallEventHook::hook(); }
void unregisterBlockFallEventHooks() { BlockFallEventHook::unhook(); }

} // namespace CauldronZero::event
