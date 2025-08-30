#pragma once

#include "CauldronZero/Global.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/events/world/block/FireBlockBurnEvent.h"
#include "CauldronZero/events/world/block/FireSpreadEvent.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/deps/core/utility/NonOwnerPointer.h"
#include "mc/util/Random.h"
#include "mc/world/actor/ai/goal/LayEggGoal.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Weather.h"
#include "mc/world/level/biome/Biome.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/BlockLegacy.h"
#include "mc/world/level/block/BurnOdds.h"
#include "mc/world/level/block/FireBlock.h"

#include "mc/world/level/block/TntBlock.h"

#include "mc/world/level/block/FlameOdds.h"
#include "mc/world/level/block/block_events/BlockQueuedTickEvent.h"
#include "mc/world/level/block/states/BlockState.h"
#include "mc/world/level/block/states/BlockStateInstance.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/level.h"
#include "mc/world/level/spawn/predicate.h"
#include "mc/world/level/storage/GameRule.h"
#include "mc\deps\core\math\Random.h"
#include "mc\util\Randomize.h"
#include "mc\world\level\block\CampfireBlock.h"
#include "mc\world\level\block\actor\BeehiveBlockActor.h"
#include "mc\world\level\block\actor\CampfireBlockActor.h"
#include "mc\world\level\block\states\vanilla_states\VanillaStates.h"
#include <chrono>
#include <ll/api/event/EventBus.h>
#include <random>
#include <string>


namespace CauldronZero::event {


LL_TYPE_INSTANCE_HOOK(
    FireBlockHook,
    ll::memory::HookPriority::Normal,
    FireBlock,
    &FireBlock::checkBurn,
    void,
    ::BlockSource&    region,
    ::BlockPos const& pos,
    int               chance,
    ::Randomize&      randomize,
    int               age
) {
    // 获取目标方块及其传统方块(Legacy)实例
    const Block&       targetBlock = region.getBlock(pos);
    const BlockLegacy& legacyBlock = *targetBlock.mLegacyBlock;

    // --- 特殊方块处理：蜂箱/蜂巢 ---
    // 1. 蜂箱处理：驱逐蜜蜂后，应该继续执行燃烧逻辑，而不是直接返回。
    if (legacyBlock.mBlockEntityType == BlockActorType::Beehive) {
        if (auto* blockActor = region.getBlockEntity(pos)) {
            static_cast<class BeehiveBlockActor*>(blockActor)->evictAll(region, false);
        }
    }
    if(targetBlock.getTypeName() == "minecraft:tnt"){
        const BlockState* explodeState = VanillaStates::getState(HashedString("explode_bit"));
        if (explodeState) {
            auto newTntBlockRef = targetBlock.setState<bool>(*explodeState, true);
            if (newTntBlockRef) {
                region.setBlock(pos, *newTntBlockRef, 3, nullptr, nullptr);
                auto& tb = region.getBlock(pos);

                auto& l   = tb.mLegacyBlock;
                auto& tnt = static_cast<class TntBlock&>(*l);
                tnt.destroy(region, pos, tb, nullptr);
                region.removeBlock(pos);
                return;
            }

        }
    }

    // 从随机化器中获取随机数生成器
    auto& random = randomize.mRandom.get()->mRandom.get().mObject;


    // BurnOdds: 方块被烧毁的几率
    auto burnOdds = legacyBlock.mBurnOdds;
    // FlameOdds: 火焰在该方块上蔓延（生成新火焰）的几率
    auto flameOdds = legacyBlock.mFlameOdds;

    if (random._genRandInt32() % chance < static_cast<uint>(burnOdds)
        ) {

        // --- 特殊方块处理：营火 ---
        // 3. 营火处理：如果是营火，则尝试点燃它并直接返回。
        if (legacyBlock.mBlockEntityType == BlockActorType::Campfire) {
            CampfireBlock::tryLightFire(region, pos, nullptr);
            return;
        }
        // --- 下雨逻辑判断 ---
        // 4. 下雨逻辑：下雨是一个独立的、优先的判断。
        // 如果正在下雨且不是寒冷生物群系，则直接摧毁方块并返回。
        bool isRaining = region.getDimension().mWeather->isPrecipitatingAt(region, pos)
                      && region.getBiome(pos).getTemperature(region, pos) > 0.15f;

        // 还有另一个随机熄灭的判定
        bool randomExtinguish = (random._genRandInt32() % (age + 10)) >= 5;

        if (isRaining || randomExtinguish) {
            // 如果满足下雨或随机熄灭条件，并且该方块确实是可燃的(BurnOdds > 0)
            if (static_cast<int>(burnOdds) > 0) {
                // 直接移除方块（模拟烧毁），不生成新火焰
                region.removeBlock(pos);
            }
            return;
        }


        // 计算新火焰的年龄
        auto newAge = age + (random._genRandInt32() % 5) / 4;
        if (newAge > 15) {
            newAge = 15;
        }

            // 获取火方块，并设置其年龄状态
            // `this` 指向的是源火方块实例
            const Block* newFireBlock = this->trySetState(
                *VanillaStates::getState(HashedString("age")), // 获取"age"这个BlockState
                newAge,
                0
            );

            if (newFireBlock) {
                // 在目标位置设置新的火方块
                region.setBlock(pos, *newFireBlock, 3, nullptr, nullptr);


            }
        
    }
    // 如果最初的随机判定失败，则不执行任何操作
}


} // namespace CauldronZero::event
