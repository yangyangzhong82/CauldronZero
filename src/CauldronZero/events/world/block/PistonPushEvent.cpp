#include "PistonPushEvent.h"
#include "CauldronZero/Logger.h"
#include "CauldronZero/SEHHandler.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/block/actor/PistonBlockActor.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"

namespace CauldronZero::event {

// --- PistonPushBeforeEvent ---
PistonBlockActor& PistonPushBeforeEvent::getPiston() const { return mPiston; }
BlockSource& PistonPushBeforeEvent::getRegion() const { return mRegion; }
BlockPos const& PistonPushBeforeEvent::getCurPos() const { return mCurPos; }
uchar PistonPushBeforeEvent::getCurBranchFacing() const { return mCurBranchFacing; }
uchar PistonPushBeforeEvent::getPistonMoveFacing() const { return mPistonMoveFacing; }

void PistonPushBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["piston"] = ll::event::serializeRefObj(mPiston);
    nbt["region"] = ll::event::serializeRefObj(mRegion);
    nbt["curPos_x"] = mCurPos.x;
    nbt["curPos_y"] = mCurPos.y;
    nbt["curPos_z"] = mCurPos.z;
    nbt["curBranchFacing"] = mCurBranchFacing;
    nbt["pistonMoveFacing"] = mPistonMoveFacing;
}

class PistonPushBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::PistonPushBeforeEvent> {};

// --- PistonPushAfterEvent ---
PistonBlockActor& PistonPushAfterEvent::getPiston() const { return mPiston; }
BlockSource& PistonPushAfterEvent::getRegion() const { return mRegion; }
BlockPos const& PistonPushAfterEvent::getCurPos() const { return mCurPos; }
uchar PistonPushAfterEvent::getCurBranchFacing() const { return mCurBranchFacing; }
uchar PistonPushAfterEvent::getPistonMoveFacing() const { return mPistonMoveFacing; }
bool PistonPushAfterEvent::getResult() const { return mResult; }

void PistonPushAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["piston"] = ll::event::serializeRefObj(mPiston);
    nbt["region"] = ll::event::serializeRefObj(mRegion);
    nbt["curPos_x"] = mCurPos.x;
    nbt["curPos_y"] = mCurPos.y;
    nbt["curPos_z"] = mCurPos.z;
    nbt["curBranchFacing"] = mCurBranchFacing;
    nbt["pistonMoveFacing"] = mPistonMoveFacing;
    nbt["result"] = mResult;
}

class PistonPushAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::PistonPushAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    PistonPushHook,
    HookPriority::Normal,
    PistonBlockActor,
    &PistonBlockActor::_attachedBlockWalker,
    bool,
    BlockSource&    region,
    BlockPos const& curPos,
    uchar           curBranchFacing,
    uchar           pistonMoveFacing
) {
    try {
        auto& piston = *this;
        auto beforeEvent = PistonPushBeforeEvent(piston, region, curPos, curBranchFacing, pistonMoveFacing);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return false;
        }

        bool result = origin(region, curPos, curBranchFacing, pistonMoveFacing);

        auto afterEvent = PistonPushAfterEvent(piston, region, curPos, curBranchFacing, pistonMoveFacing, result);
        ll::event::EventBus::getInstance().publish(afterEvent);

        return result;
    } catch (const SEH_Exception& e) {
        logger.warn(
            "PistonPushHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(region, curPos, curBranchFacing, pistonMoveFacing);
    } catch (const std::exception& e) {
        logger.warn("PistonPushHook 发生 C++ 异常: {}", e.what());
        return origin(region, curPos, curBranchFacing, pistonMoveFacing);
    } catch (...) {
        logger.warn("PistonPushHook 发生未知异常！");
        return origin(region, curPos, curBranchFacing, pistonMoveFacing);
    }
}

void registerPistonPushEventHooks() { PistonPushHook::hook(); }
void unregisterPistonPushEventHooks() { PistonPushHook::unhook(); }

} // namespace CauldronZero::event
