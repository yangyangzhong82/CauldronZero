#include "CauldronZero/events/world/block/LiquidBlockSpread.h"
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

// --- LiquidSpreadBeforeEvent ---
BlockSource&    LiquidSpreadBeforeEvent::getRegion() const { return mRegion; }
const BlockPos& LiquidSpreadBeforeEvent::getPos() const { return mPos; }
int             LiquidSpreadBeforeEvent::getNeighbor() const { return mNeighbor; }
const BlockPos& LiquidSpreadBeforeEvent::getFlowFromPos() const { return mFlowFromPos; }
unsigned char   LiquidSpreadBeforeEvent::getFlowFromDirection() const { return mFlowFromDirection; }

void LiquidSpreadBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["dimension"]        = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]              = ListTag{mPos.x, mPos.y, mPos.z};
    nbt["neighbor"]         = mNeighbor;
    nbt["flowFromPos"]      = ListTag{mFlowFromPos.x, mFlowFromPos.y, mFlowFromPos.z};
    nbt["flowFromDirection"] = mFlowFromDirection;
}

class LiquidSpreadBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, LiquidSpreadBeforeEvent> {};

// --- LiquidSpreadAfterEvent ---
BlockSource&    LiquidSpreadAfterEvent::getRegion() const { return mRegion; }
const BlockPos& LiquidSpreadAfterEvent::getPos() const { return mPos; }
int             LiquidSpreadAfterEvent::getNeighbor() const { return mNeighbor; }
const BlockPos& LiquidSpreadAfterEvent::getFlowFromPos() const { return mFlowFromPos; }
unsigned char   LiquidSpreadAfterEvent::getFlowFromDirection() const { return mFlowFromDirection; }

void LiquidSpreadAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["dimension"]        = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]              = ListTag{mPos.x, mPos.y, mPos.z};
    nbt["neighbor"]         = mNeighbor;
    nbt["flowFromPos"]      = ListTag{mFlowFromPos.x, mFlowFromPos.y, mFlowFromPos.z};
    nbt["flowFromDirection"] = mFlowFromDirection;
}

class LiquidSpreadAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, LiquidSpreadAfterEvent> {};


// --- 挂钩 ---
LL_TYPE_INSTANCE_HOOK(
    LiquidSpreadEventHook,
    ll::memory::HookPriority::Normal,
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
        auto beforeEvent = LiquidSpreadBeforeEvent(region, pos, neighbor, flowFromPos, flowFromDirection);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(region, pos, neighbor, flowFromPos, flowFromDirection);

        auto afterEvent = LiquidSpreadAfterEvent(region, pos, neighbor, flowFromPos, flowFromDirection);
        ll::event::EventBus::getInstance().publish(afterEvent);

    } catch (const SEH_Exception& e) {
        logger.error(
            "在 LiquidSpreadEventHook 中捕获到 SEH 异常: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(region, pos, neighbor, flowFromPos, flowFromDirection);
        throw;
    } catch (const std::exception& e) {
        logger.error("在 LiquidSpreadEventHook 中捕获到标准异常: {}", e.what());
        origin(region, pos, neighbor, flowFromPos, flowFromDirection);
        throw;
    } catch (...) {
        logger.error("在 LiquidSpreadEventHook 中捕获到未知异常");
        origin(region, pos, neighbor, flowFromPos, flowFromDirection);
        throw;
    }
}

void registerLiquidSpreadEventHooks() { LiquidSpreadEventHook::hook(); }
void unregisterLiquidSpreadEventHooks() { LiquidSpreadEventHook::unhook(); }

} // namespace CauldronZero::event
