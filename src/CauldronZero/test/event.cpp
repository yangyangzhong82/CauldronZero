#pragma once
#include "CauldronZero/events/world/ExplosionEvent.h"
#include "CauldronZero/events/world/actor/MobHurtEffectEvent.h"
#include "CauldronZero/events/world/actor/player/PlayerEditSignEvent.h" // Added for PlayerEditSignEvent

#include "CauldronZero/events/world/block/ItemFrameBlockEvent.h" // Added
#include "CauldronZero/logger.h"
#include "ll/api/event/EventRefObjSerializer.h" // Added
#include "ll\api\event\EventBus.h"
#include "mc/nbt/CompoundTag.h"        // Added
#include "mc/nbt/CompoundTagVariant.h" // Added
#include "mc/world/actor/Actor.h"
#include "mc/world/actor/ActorDefinitionIdentifier.h"
#include "mc/world/actor/Mob.h"
#include "mc/world/actor/player/Player.h" // Added
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h" // Added
#include "mc/world/level/Explosion.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/ItemFrameBlock.h" // Added

#include "CauldronZero/events/server/ClientLoginEvent.h" // Added for ClientLoginBeforeEvent

namespace CauldronZero::event {


void registerTestEventListeners() {
    logger.info("registerTestEventListeners: 注册测试事件监听器.");

    // 1. ExplosionBeforeEvent
    ll::event::EventBus::getInstance().emplaceListener<CauldronZero::event::ExplosionBeforeEvent>(
        [](CauldronZero::event::ExplosionBeforeEvent& event) {
            auto& explosion = event.getExplosion();
            logger.info(
                "ExplosionBeforeEvent: , Pos=({}, {}, {}), Radius={}",
                explosion.mPos->x,
                explosion.mPos->y,
                explosion.mPos->z,
                explosion.mMaxResistance

            );
            CompoundTag nbt;
            event.serialize(nbt);
            logger.info("ExplosionBeforeEvent NBT: {}", nbt.toString());
        }
    );




    // 6. ItemFrameBlockAttackBeforeEvent
    ll::event::EventBus::getInstance().emplaceListener<CauldronZero::event::ItemFrameBlockAttackBeforeEvent>(
        [](CauldronZero::event::ItemFrameBlockAttackBeforeEvent& event) {
            const auto& player = event.getPlayer();
            const auto& pos    = event.getPos();
            logger.info(
                "ItemFrameBlockAttackBeforeEvent: Player={}, Pos=({}, {}, {})",
                player.getNameTag(),
                pos.x,
                pos.y,
                pos.z
            );
            CompoundTag nbt;
            event.serialize(nbt);
            logger.info("ItemFrameBlockAttackBeforeEvent NBT: {}", nbt.toString());
        }
    );

    // 7. MobHurtEffectEvent
    ll::event::EventBus::getInstance().emplaceListener<CauldronZero::event::MobHurtEffectEvent>(
        [](CauldronZero::event::MobHurtEffectEvent& event) {
            auto&       mob          = event.getMob();
            const auto& source       = event.getSource();
            auto&       damage       = event.getDamage();
            auto*       damageSource = event.getDamageSource();
            logger.info(
                "MobHurtEffectEvent: MobType={}, Damage={}, SourceCause={}, DamageSource={}",
                mob.getTypeName(),
                damage,
                (int)source.mCause,
                damageSource ? damageSource->getTypeName() : "null"
            );
            CompoundTag nbt;
            event.serialize(nbt);
            logger.info("MobHurtEffectEvent NBT: {}", nbt.toString());
        }
    );

    // 8. PlayerEditSignBeforeEvent
    ll::event::EventBus::getInstance().emplaceListener<CauldronZero::event::PlayerEditSignBeforeEvent>(
        [](CauldronZero::event::PlayerEditSignBeforeEvent& event) {
            logger.info("PlayerEditSignBeforeEvent 监听器被调用 (简化版).");
            // CompoundTag nbt;
            // event.serialize(nbt);
            // logger.info("PlayerEditSignBeforeEvent NBT: {}", nbt.toString());
            // event.cancel(); // 可以取消事件
        }
    );
    ll::event::EventBus::getInstance().emplaceListener<CauldronZero::event::PlayerEditSignAfterEvent>(
        [](CauldronZero::event::PlayerEditSignAfterEvent& event) {
            logger.info("PlayerEditSignBeforeEvent 114514 (简化版).");
            // CompoundTag nbt;
            // event.serialize(nbt);
            // logger.info("PlayerEditSignBeforeEvent NBT: {}", nbt.toString());
            // event.cancel(); // 可以取消事件
        }
    );

}
} // namespace CauldronZero::event
