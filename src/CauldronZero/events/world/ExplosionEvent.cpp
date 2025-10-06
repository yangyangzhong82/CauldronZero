#include "ExplosionEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "mc\world\level\BlockSource.h"
#include "ll/api/memory/Hook.h"
#include "mc/legacy/ActorUniqueID.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"

#include "mc/world/level/Explosion.h"
#include "mc\world\level\block/BlockType.h"
#include <cstddef>
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>


namespace CauldronZero::event {

// --- ExplosionBeforeEvent ---
Explosion& ExplosionBeforeEvent::getExplosion() const { return mExplosion; }

void ExplosionBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["explosion"] = ll::event::serializeRefObj(mExplosion);


    nbt["radius"]                         = mExplosion.mRadius;
    nbt["fire"]                           = mExplosion.mFire;
    nbt["breaking"]                       = mExplosion.mBreaking;
    nbt["allowUnderwater"]                = mExplosion.mAllowUnderwater;
    nbt["cantoggleblocks"]                = mExplosion.mCanToggleBlocks;
    nbt["damagescaling"]                  = mExplosion.mDamageScaling;
    nbt["ignoreblockexplosionresistance"] = mExplosion.mIgnoreBlockExplosionResistance;
    nbt["particletype"]                   = static_cast<int>(mExplosion.mParticleType);
    nbt["soundExplosiontype"]             = static_cast<int>(mExplosion.mSoundExplosionType);
    nbt["sourceid"]                       = mExplosion.mSourceID->rawID;
    nbt["maxResistance"]                  = mExplosion.mMaxResistance;
    if (getExplosion().mInWaterOverride->has_value()) {
        nbt["inwateroverride"] = getExplosion().mInWaterOverride->value();
    }
    if (getExplosion().mTotalDamageOverride->has_value()) {
        nbt["totaldamageoverride"] = getExplosion().mTotalDamageOverride->value();
    }
    nbt["x"]     = mExplosion.mPos->x;
   nbt["y"]     = mExplosion.mPos->y;
    nbt["z"]     = mExplosion.mPos->z;
    nbt["dimension"] = static_cast<int>(mExplosion.mRegion.getDimensionId());
}

// --- ExplosionAfterEvent ---
Explosion& ExplosionAfterEvent::getExplosion() const { return mExplosion; }

void ExplosionAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["explosion"] = ll::event::serializeRefObj(mExplosion);


    nbt["radius"]                         = mExplosion.mRadius;
    nbt["fire"]                           = mExplosion.mFire;
    nbt["breaking"]                       = mExplosion.mBreaking;
    nbt["allowUnderwater"]                = mExplosion.mAllowUnderwater;
    nbt["cantoggleblocks"]                = mExplosion.mCanToggleBlocks;
    nbt["damagescaling"]                  = mExplosion.mDamageScaling;
    nbt["ignoreblockexplosionresistance"] = mExplosion.mIgnoreBlockExplosionResistance;
    nbt["particletype"]                   = static_cast<int>(mExplosion.mParticleType);
    nbt["soundExplosiontype"]             = static_cast<int>(mExplosion.mSoundExplosionType);
    nbt["sourceid"]                       = mExplosion.mSourceID->rawID;
    nbt["maxResistance"]                  = mExplosion.mMaxResistance;
    if (getExplosion().mInWaterOverride->has_value()) {
        nbt["inwateroverride"] = getExplosion().mInWaterOverride->value();
    }
    if (getExplosion().mTotalDamageOverride->has_value()) {
        nbt["totaldamageoverride"] = getExplosion().mTotalDamageOverride->value();
    }
    nbt["x"]         = mExplosion.mPos->x;
    nbt["y"]         = mExplosion.mPos->y;
    nbt["z"]         = mExplosion.mPos->z;
    nbt["dimension"] = static_cast<int>(mExplosion.mRegion.getDimensionId());
}


class ExplosionBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ExplosionBeforeEvent> {};

class ExplosionAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ExplosionAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    ExplosionHook,
    ll::memory::HookPriority::Normal,
    Explosion,
    &Explosion::explode,
    bool,
    ::IRandom& ranomd
) {
    try {
        auto& explosion = *this;
        auto  event     = CauldronZero::event::ExplosionBeforeEvent(explosion);
        explosion.mAffectedBlocks->clear(); // 在推送事件前清空 mAffectedBlocks
        ll::event::EventBus::getInstance().publish(event);

        if (event.isCancelled()) {
            return false;
        }

        auto const result = origin(ranomd);

        auto afterEvent = CauldronZero::event::ExplosionAfterEvent(explosion);
        ll::event::EventBus::getInstance().publish(afterEvent);

        return result;
    } catch (const SEH_Exception& e) {
        logger.warn(
            "ExplosionHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(ranomd);
    } catch (const std::exception& e) {
        logger.warn("ExplosionHook 发生 C++ 异常: {}", e.what());
        return origin(ranomd);
    } catch (...) {
        logger.warn("ExplosionHook 发生未知异常！");
        return origin(ranomd);
    }
}
void registerExplosionEventHooks() { ExplosionHook::hook(); }

void unregisterExplosionEventHooks() { ExplosionHook::unhook(); }

} // namespace CauldronZero::event
