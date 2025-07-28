#include "ProjectileHitEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/entity/components_json_legacy/ProjectileComponent.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/phys/HitResult.h"
#include "mc/world/phys/HitResultType.h"


namespace CauldronZero::event {

// --- ProjectileHitBeforeEvent ---
Actor&           ProjectileHitBeforeEvent::getOwner() const { return mOwner; }
HitResult const& ProjectileHitBeforeEvent::getHitResult() const { return mHitResult; }

void ProjectileHitBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["owner"] = ll::event::serializeRefObj(mOwner);

    // Serialize HitResult
    nbt["hitResult_type"] = static_cast<int>(mHitResult.mType);
    nbt["hitResult_pos_x"] = mHitResult.mPos.x;
    nbt["hitResult_pos_y"] = mHitResult.mPos.y;
    nbt["hitResult_pos_z"] = mHitResult.mPos.z;

    if (mHitResult.mType == HitResultType::Entity) {
        Actor* hitEntity = mHitResult.getEntity();
        if (hitEntity) {
            nbt["hitResult_entity"] = ll::event::serializeRefObj(*hitEntity);
        }
    } else if (mHitResult.mType == HitResultType::Tile) {
        nbt["hitResult_block_x"] = mHitResult.mBlock.x;
        nbt["hitResult_block_y"] = mHitResult.mBlock.y;
        nbt["hitResult_block_z"] = mHitResult.mBlock.z;
        nbt["hitResult_facing"] = mHitResult.mFacing;
    }
}

class ProjectileHitBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, ProjectileHitBeforeEvent> {};


// --- ProjectileHitAfterEvent ---
Actor&           ProjectileHitAfterEvent::getOwner() const { return mOwner; }
HitResult const& ProjectileHitAfterEvent::getHitResult() const { return mHitResult; }

void ProjectileHitAfterEvent::serialize(CompoundTag& nbt) const {
    nbt["owner"] = ll::event::serializeRefObj(mOwner);

    // Serialize HitResult
    nbt["hitResult_type"] = static_cast<int>(mHitResult.mType);
    nbt["hitResult_pos_x"] = mHitResult.mPos.x;
    nbt["hitResult_pos_y"] = mHitResult.mPos.y;
    nbt["hitResult_pos_z"] = mHitResult.mPos.z;

    if (mHitResult.mType == HitResultType::Entity) {
        Actor* hitEntity = mHitResult.getEntity();
        if (hitEntity) {
            nbt["hitResult_entity"] = ll::event::serializeRefObj(*hitEntity);
        }
    } else if (mHitResult.mType == HitResultType::Tile) {
        nbt["hitResult_block_x"] = mHitResult.mBlock.x;
        nbt["hitResult_block_y"] = mHitResult.mBlock.y;
        nbt["hitResult_block_z"] = mHitResult.mBlock.z;
        nbt["hitResult_facing"] = mHitResult.mFacing;
    }
}

class ProjectileHitAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, ProjectileHitAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    onProjectileHitHook,
    ll::memory::HookPriority::Normal,
    ProjectileComponent,
    &::ProjectileComponent::onHit,
    void,
    Actor&           owner,
    HitResult const& res
) {
    try {
        auto beforeEvent = ProjectileHitBeforeEvent(owner, res);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(owner, res);

        auto afterEvent = ProjectileHitAfterEvent(owner, res);
        ll::event::EventBus::getInstance().publish(afterEvent);

    } catch (const SEH_Exception& e) {
        logger.warn(
            "onProjectileHitHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(owner, res);
    } catch (const std::exception& e) {
        logger.warn("onProjectileHitHook 发生 C++ 异常: {}", e.what());
        origin(owner, res);
    } catch (...) {
        logger.warn("onProjectileHitHook 发生未知异常！");
        origin(owner, res);
    }
}

void registerProjectileHitEventHooks() { onProjectileHitHook::hook(); }
void unregisterProjectileHitEventHooks() { onProjectileHitHook::unhook(); }

} // namespace CauldronZero::event
