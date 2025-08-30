#include "SculkSpreadEvent.h"
#include "CauldronZero/Logger.h"
#include "CauldronZero/SEHHandler.h"
#include "ll/api/memory/Hook.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include <mc/world/level/WorldBlockTarget.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/block/MultifaceSpreader.h>
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/BlockPos.h"
#include <optional>
#include <utility> // For std::pair

namespace CauldronZero::event {

using GetSpreadResultType = ::std::optional<::std::pair<::BlockPos const, uchar const>>;

// --- SculkSpreadBeforeEvent ---
MultifaceSpreader& SculkSpreadBeforeEvent::getSpreader() const { return mSpreader; }
IBlockWorldGenAPI& SculkSpreadBeforeEvent::getTarget() const { return mTarget; }
Block const& SculkSpreadBeforeEvent::getSelfBlock() const { return mSelfBlock; }
Block const& SculkSpreadBeforeEvent::getBlock() const { return mBlock; }
BlockPos const& SculkSpreadBeforeEvent::getPos() const { return mPos; }
uchar SculkSpreadBeforeEvent::getStartingFace() const { return mStartingFace; }
uchar SculkSpreadBeforeEvent::getSpreadDirection() const { return mSpreadDirection; }

void SculkSpreadBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["spreader"] = ll::event::serializeRefObj(mSpreader);
    // mTarget 无法直接序列化
    nbt["selfBlock"] = ll::event::serializeRefObj(mSelfBlock);
    nbt["block"] = ll::event::serializeRefObj(mBlock);
    nbt["pos_x"] = mPos.x;
    nbt["pos_y"] = mPos.y;
    nbt["pos_z"] = mPos.z;
    nbt["startingFace"] = mStartingFace;
    nbt["spreadDirection"] = mSpreadDirection;
}

class SculkSpreadBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::SculkSpreadBeforeEvent> {};

// --- SculkSpreadAfterEvent ---
MultifaceSpreader& SculkSpreadAfterEvent::getSpreader() const { return mSpreader; }
IBlockWorldGenAPI& SculkSpreadAfterEvent::getTarget() const { return mTarget; }
Block const& SculkSpreadAfterEvent::getSelfBlock() const { return mSelfBlock; }
Block const& SculkSpreadAfterEvent::getBlock() const { return mBlock; }
BlockPos const& SculkSpreadAfterEvent::getPos() const { return mPos; }
uchar SculkSpreadAfterEvent::getStartingFace() const { return mStartingFace; }
uchar SculkSpreadAfterEvent::getSpreadDirection() const { return mSpreadDirection; }
std::optional<std::pair<BlockPos const, uchar const>> SculkSpreadAfterEvent::getResult() const { return mResult; }

void SculkSpreadAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["spreader"] = ll::event::serializeRefObj(mSpreader);
    // mTarget 无法直接序列化
    nbt["selfBlock"] = ll::event::serializeRefObj(mSelfBlock);
    nbt["block"] = ll::event::serializeRefObj(mBlock);
    nbt["pos_x"] = mPos.x;
    nbt["pos_y"] = mPos.y;
    nbt["pos_z"] = mPos.z;
    nbt["startingFace"] = mStartingFace;
    nbt["spreadDirection"] = mSpreadDirection;
    if (mResult.has_value()) {
        nbt["result_pos_x"] = mResult->first.x;
        nbt["result_pos_y"] = mResult->first.y;
        nbt["result_pos_z"] = mResult->first.z;
        nbt["result_face"] = mResult->second;
    }
}

class SculkSpreadAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::SculkSpreadAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    SculkSpreadHook,
    HookPriority::Normal,
    MultifaceSpreader,
    &MultifaceSpreader::getSpreadFromFaceTowardDirection, // 移除 $ 前缀，因为它不是虚函数
    GetSpreadResultType, // 使用别名作为返回类型
    ::IBlockWorldGenAPI& target,
    ::Block const&       self,
    ::Block const&       block,
    ::BlockPos const&    pos,
    uchar                startingFace,
    uchar                spreadDirection
) {
    try {
        auto& spreader = *this;
        auto beforeEvent = SculkSpreadBeforeEvent(spreader, target, self, block, pos, startingFace, spreadDirection);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return std::nullopt; // 事件取消时返回空 optional
        }

        auto result = origin(target, self, block, pos, startingFace, spreadDirection);

        auto afterEvent = SculkSpreadAfterEvent(spreader, target, self, block, pos, startingFace, spreadDirection, result);
        ll::event::EventBus::getInstance().publish(afterEvent);

        return result;
    } catch (const SEH_Exception& e) {
        logger.warn(
            "SculkSpreadHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(target, self, block, pos, startingFace, spreadDirection);
    } catch (const std::exception& e) {
        logger.warn("SculkSpreadHook 发生 C++ 异常: {}", e.what());
        return origin(target, self, block, pos, startingFace, spreadDirection);
    } catch (...) {
        logger.warn("SculkSpreadHook 发生未知异常！");
        return origin(target, self, block, pos, startingFace, spreadDirection);
    }
}

void registerSculkSpreadEventHooks() { SculkSpreadHook::hook(); }
void unregisterSculkSpreadEventHooks() { SculkSpreadHook::unhook(); }

} // namespace CauldronZero::event
