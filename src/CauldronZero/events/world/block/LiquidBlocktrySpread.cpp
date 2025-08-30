#include "CauldronZero/events/world/block/LiquidBlockTrySpread.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/memory/Hook.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/LiquidBlock.h"


namespace CauldronZero::event {

// --- LiquidBlockTrySpreadBeforeEvent ---
BlockSource&    LiquidBlockTrySpreadBeforeEvent::getRegion() const { return mRegion; }
const BlockPos& LiquidBlockTrySpreadBeforeEvent::getPos() const { return mPos; }
int             LiquidBlockTrySpreadBeforeEvent::getNeighbor() const { return mNeighbor; }
const BlockPos& LiquidBlockTrySpreadBeforeEvent::getFlowFromPos() const { return mFlowFromPos; }
unsigned char   LiquidBlockTrySpreadBeforeEvent::getFlowFromDirection() const { return mFlowFromDirection; }

void LiquidBlockTrySpreadBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["dimension"]         = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]               = ListTag{mPos.x, mPos.y, mPos.z};
    nbt["neighbor"]          = mNeighbor;
    nbt["flowFromPos"]       = ListTag{mFlowFromPos.x, mFlowFromPos.y, mFlowFromPos.z};
    nbt["flowFromDirection"] = mFlowFromDirection;
}

class LiquidBlockTrySpreadBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, LiquidBlockTrySpreadBeforeEvent> {};

// --- LiquidBlockTrySpreadAfterEvent ---
BlockSource&    LiquidBlockTrySpreadAfterEvent::getRegion() const { return mRegion; }
const BlockPos& LiquidBlockTrySpreadAfterEvent::getPos() const { return mPos; }
int             LiquidBlockTrySpreadAfterEvent::getNeighbor() const { return mNeighbor; }
const BlockPos& LiquidBlockTrySpreadAfterEvent::getFlowFromPos() const { return mFlowFromPos; }
unsigned char   LiquidBlockTrySpreadAfterEvent::getFlowFromDirection() const { return mFlowFromDirection; }

void LiquidBlockTrySpreadAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["dimension"]         = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]               = ListTag{mPos.x, mPos.y, mPos.z};
    nbt["neighbor"]          = mNeighbor;
    nbt["flowFromPos"]       = ListTag{mFlowFromPos.x, mFlowFromPos.y, mFlowFromPos.z};
    nbt["flowFromDirection"] = mFlowFromDirection;
}

class LiquidBlockTrySpreadAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, LiquidBlockTrySpreadAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    LiquidTrySpreadEventHook,
    HookPriority::Normal,
    LiquidBlock,
    &LiquidBlock::_trySpreadTo,
    void,
    ::BlockSource&    region,
    ::BlockPos const& pos,
    int               neighbor,
    ::BlockPos const& flowFromPos,
    uchar             flowFromDirection
) {
    try {
        auto beforeEvent = LiquidBlockTrySpreadBeforeEvent(region, pos, neighbor, flowFromPos, flowFromDirection);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(region, pos, neighbor, flowFromPos, flowFromDirection);

        auto afterEvent = LiquidBlockTrySpreadAfterEvent(region, pos, neighbor, flowFromPos, flowFromDirection);
        ll::event::EventBus::getInstance().publish(afterEvent);

    } catch (const SEH_Exception& e) {
        logger.error(
            "在 LiquidTrySpreadEventHook 中捕获到 SEH 异常: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(region, pos, neighbor, flowFromPos, flowFromDirection);
        throw;
    } catch (const std::exception& e) {
        logger.error("在 LiquidTrySpreadEventHook 中捕获到标准异常: {}", e.what());
        origin(region, pos, neighbor, flowFromPos, flowFromDirection);
        throw;
    } catch (...) {
        logger.error("在 LiquidTrySpreadEventHook 中捕获到未知异常");
        origin(region, pos, neighbor, flowFromPos, flowFromDirection);
        throw;
    }
}

void registerLiquidTrySpreadEventHooks() { LiquidTrySpreadEventHook::hook(); }
void unregisterLiquidTrySpreadEventHooks() { LiquidTrySpreadEventHook::unhook(); }

} // namespace CauldronZero::event
