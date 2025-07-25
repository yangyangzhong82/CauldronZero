#pragma once

#include "FireBlockEvent.h"
#include "CauldronZero/Global.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/events/world/block/FireBlockBurnEvent.h"
#include "CauldronZero/events/world/block/FireSpreadEvent.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/deps/core/string/HashedString.h"
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
#include "mc/world/level/block/block_events/BlockQueuedTickEvent.h"
#include "mc/world/level/block/states/BlockState.h"
#include "mc/world/level/block/states/BlockStateInstance.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/level.h"
#include "mc/world/level/spawn/predicate.h"
#include "mc/world/level/storage/GameRule.h"
#include "mc\world\level\block\CampfireBlock.h"
#include "mc\world\level\block\actor\BeehiveBlockActor.h"
#include "mc\world\level\block\actor\CampfireBlockActor.h"
#include <chrono>
#include <ll/api/event/EventBus.h>
#include <random>
#include <string>



namespace CauldronZero::event {
LL_TYPE_INSTANCE_HOOK(
    FireBlockEventHook,
    ll::memory::HookPriority::Normal,
    FireBlock,
    &FireBlock::tick,
    void,
    ::BlockEvents::BlockQueuedTickEvent& eventData
) {
    try {
        auto&    region    = eventData.mRegion;     // 方块所在区域
        auto     pos       = eventData.mPos.get();  // 火焰方块的坐标
        auto&    dim       = region.getDimension(); // 所在维度
        Level*   level     = &region.getLevel();    // 当前存档
        Weather* weather   = dim.mWeather.get();    // 天气控制器
        auto&    fireblock = eventData.getBlock();  // 当前火焰方块实例
        Random&  random    = eventData.mRandom;     // 随机数生成器



        // 获取火焰方块的 "age" 状态值，如果获取失败则默认为 0
        const Block&       currentFireBlock = eventData.getBlock();
        std::optional<int> stateOpt   = currentFireBlock.getState<int>(1); // 假设 BlockState ID 1 是火焰年龄
        int                currentAge = stateOpt.value_or(0);

        // --- 灵魂火生成 ---
        // 尝试在当前位置生成灵魂火，如果成功则终止后续的普通火焰逻辑
        if (this->_trySpawnSoulFire(region, pos)) {
            logger.debug("灵魂火在 {} 生成成功。", pos);
            return;
        }

        // --- 环境检查 ---
        // 检查火焰下方的方块，判断是否为永恒火焰
        ::BlockPos     belowPos   = {pos.x, pos.y - 1, pos.z};
        ::Block const& blockBelow = region.getBlock(belowPos);
        std::string    blockName  = blockBelow.getLegacyBlock().getTypeName();

        // 检查是否为地狱岩或岩浆块
        bool isEternalFire = (blockName == "minecraft:netherrack" || blockName == "minecraft:magma_block");

        // 如果在末地，基岩也支持永恒火焰
        if (!isEternalFire && dim.getDimensionId() == 2) { // 假设末地ID为2
            if (blockName == "minecraft:bedrock") {
                isEternalFire = true;
            }
        }

        // 检查当前生物群系是否潮湿，潮湿会影响火焰的燃烧和蔓延
        ::Biome const& currentBiome = region.getBiome(pos);
        bool           isHumidBiome = currentBiome.isHumid();

        // 如果正在下雨，则直接熄灭火焰
        if (weather->isPrecipitatingAt(region, pos)) {
            region.removeBlock(pos);
            return;
        }

        // --- 孤立火焰熄灭规则 ---
        // 定义六个方向的坐标偏移
        ::BlockPos sixDirections[] = {
            {0,  1,  0 },
            {0,  -1, 0 }, // 上、下
            {1,  0,  0 },
            {-1, 0,  0 }, // 东、西
            {0,  0,  1 },
            {0,  0,  -1}  // 南、北
        };

        // 检查火焰周围六个方向是否有可燃方块
        bool isAdjacentToFlammable = false;
        for (int i = 0; i < 6; ++i) {
            ::BlockPos         checkPos    = pos + sixDirections[i];
            const BlockLegacy& checkLegacy = region.getBlock(checkPos).getLegacyBlock();
            if (static_cast<int>(checkLegacy.mBurnOdds) > 0) {
                isAdjacentToFlammable = true;
                break;
            }
        }

        // 如果火焰没有毗邻任何可燃方块，且其年龄大于3，则立即熄灭
        if (!isAdjacentToFlammable && currentAge > 3) {
            region.removeBlock(pos);
            logger.debug("火焰 @ {} 因周围无可燃物且年龄 > 3 而熄灭。", pos);
            return;
        }

        // --- 直接燃烧逻辑 ---
        bool didBurnBlock = false; // 标记此 tick 中是否有方块被烧毁

        // 遍历六个方向，尝试点燃邻近的方块
        for (int i = 0; i < 6; ++i) {
            ::BlockPos         neighborPos         = pos + sixDirections[i];
            const BlockLegacy& neighborBlockLegacy = region.getBlock(neighborPos).getLegacyBlock();
            int                burnOdds            = static_cast<int>(neighborBlockLegacy.mBurnOdds);

            if (burnOdds > 0) {
                // --- 新增：检查是否为营火并尝试点燃 ---
                std::string neighborBlockName = neighborBlockLegacy.getTypeName();
                if (neighborBlockName == "minecraft:campfire" || neighborBlockName == "minecraft:soul_campfire") {
                    /*
                    if (CampfireBlock::tryLightFire(region, neighborPos, nullptr)) {
                        logger.debug("营火 @ {} 被火焰点燃。", neighborPos);
                        didBurnBlock = true; // 标记已处理
                        continue;            // 处理完营火后，跳过后续的常规燃烧逻辑
                    }
                        */
                }
                // --- 检查结束 ---

                // 根据方向和生物群系湿度调整燃烧基础概率
                int baseChance = (sixDirections[i].y != 0) ? 250 : 300; // 上下方向基础值为 250，侧面为 300
                if (isHumidBiome) {
                    baseChance = (sixDirections[i].y != 0) ? 200 : 250; // 潮湿生物群系中，上下为 200，侧面为 250
                }

                // 应用烧毁几率修改器
                float finalBurnChance = CauldronZero::Global::getInstance().getConfig().get<float>(
                    "features.fire_spread_control.burn_chance_multiplier",
                    1.0f
                );

                // 如果随机检定通过，则烧毁方块
                if (random.nextInt(baseChance) < finalBurnChance) {
                    // --- 事件触发：方块燃烧前 ---
                    auto burnEvent = CauldronZero::event::FireBlockBurnBeforeEvent(
                        region,
                        pos,
                        neighborPos,
                        region.getBlock(neighborPos)
                    );
                    ll::event::EventBus::getInstance().publish(burnEvent);

                    if (burnEvent.isCancelled()) {
                        logger.debug("方块 @ {} 的燃烧被事件取消。", neighborPos);
                        continue; // 跳过此方块的燃烧
                    }

                    // 在方块被烧毁前，检查它是否是蜂巢
                    BlockActor* blockEntity = region.getBlockEntity(neighborPos);
                    if (blockEntity && blockEntity->mType == BlockActorType::Beehive) {
                        // 如果是蜂巢，先驱赶蜜蜂
                        static_cast<BeehiveBlockActor*>(blockEntity)->evictAll(region, false);
                        logger.debug("蜂巢 @ {} 的蜜蜂已被驱赶。", neighborPos);
                    }

                    region.removeBlock(neighborPos);
                    didBurnBlock = true;
                    logger
                        .debug("方块 @ {} 被烧毁 (燃烧系数: {}, 修改后: {})。", neighborPos, burnOdds, finalBurnChance);

                    // --- 事件触发：方块燃烧后 ---
                    auto burnAfterEvent = CauldronZero::event::FireBlockBurnAfterEvent(region, pos, neighborPos);
                    ll::event::EventBus::getInstance().publish(burnAfterEvent);

                    // 自定义规则：方块烧毁后，有 50% 的几率在原地生成新的火焰
                    if (!weather->isPrecipitatingAt(region, neighborPos) && random.nextInt(100) < 50) {
                        auto fireBlockLegacyOpt = BlockLegacy::tryGetFromRegistry("minecraft:fire");
                        if (fireBlockLegacyOpt.has_value()) {
                            BlockLegacy const& fireLegacy    = fireBlockLegacyOpt.value();
                            auto               ageBlockState = fireLegacy.getBlockState(HashedString("age"));
                            // 尝试将新火焰的年龄设置为与当前火焰相同
                            if (ageBlockState) {
                                auto newFireBlockOpt =
                                    fireLegacy.mDefaultState->setState<int>(*ageBlockState, currentAge);
                                if (newFireBlockOpt.has_value()) {
                                    region.setBlock(neighborPos, newFireBlockOpt.value(), 3, nullptr, nullptr);
                                } else {
                                    region.setBlock(neighborPos, *fireLegacy.mDefaultState, 3, nullptr, nullptr);
                                }
                            } else {
                                region.setBlock(neighborPos, *fireLegacy.mDefaultState, 3, nullptr, nullptr);
                            }
                        }
                        logger.debug("新火焰 (年龄: {}) 在 {} 生成。", currentAge, neighborPos);
                    }
                }
            }
        }

        // --- 跳跃蔓延逻辑 ---
        // 在一个 3x5x3 的区域内尝试蔓延
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dz = -1; dz <= 1; ++dz) {
                for (int dy = -1; dy <= 4; ++dy) {
                    if (dx == 0 && dz == 0 && dy == 0) continue; // 跳过火焰自身

                    ::BlockPos spreadPos = {pos.x + dx, pos.y + dy, pos.z + dz};

                    // 只有当目标位置是空气时才考虑蔓延
                    if (region.getBlock(spreadPos).isAir()) {
                        // 检查蔓延目标位置周围是否有可燃方块，这是蔓延的前提
                        bool isFlammableNearby = false;
                        for (int i = 0; i < 6; ++i) {
                            ::BlockPos neighborOfSpreadPos = spreadPos + sixDirections[i];
                            if (static_cast<int>(region.getBlock(neighborOfSpreadPos).getLegacyBlock().mBurnOdds) > 0) {
                                isFlammableNearby = true;
                                break;
                            }
                        }

                        if (isFlammableNearby) {
                            // --- 根据 Wiki 公式计算蔓延概率 ---
                            // 获取周围方块的最大引燃率
                            float maxNeighborBurnOdds = 0.0f;
                            for (int i = 0; i < 6; ++i) {
                                ::BlockPos neighborOfSpreadPos = spreadPos + sixDirections[i];
                                maxNeighborBurnOdds            = std::max(
                                    maxNeighborBurnOdds,
                                    (float)region.getBlock(neighborOfSpreadPos).getLegacyBlock().mBurnOdds
                                );
                            }

                            // 困难难度 d = 3
                            const int difficulty = 2;

                            // 计算引燃度
                            float encouragement = (maxNeighborBurnOdds + 7.0f * difficulty + 15.0f);
                            float flameFactor   = (float)(currentAge + 42);
                            float burnChance    = encouragement / flameFactor;

                            // 根据距离调整基础引燃度
                            int baseFlammability = 130;
                            if (dy == 2) baseFlammability = 250;
                            if (dy == 3) baseFlammability = 350;
                            if (dy == 4) baseFlammability = 450;

                            // 计算最终蔓延概率
                            float spreadChance = burnChance / (float)baseFlammability;

                            if (isHumidBiome) {
                                spreadChance *= 0.5f; // 潮湿生物群系概率减半
                            }

                            // 应用蔓延几率修改器
                            spreadChance *= CauldronZero::Global::getInstance().getConfig().get<float>(
                                "features.fire_spread_control.spread_chance_multiplier",
                                1.0f
                            );

                            // 如果随机检定通过，则生成新火焰
                            if (spreadChance > 0 && random.nextFloat() < spreadChance) {
                                // --- 事件触发：火焰蔓延前 ---
                                auto spreadEvent = CauldronZero::event::FireSpreadBeforeEvent(region, pos, spreadPos);
                                ll::event::EventBus::getInstance().publish(spreadEvent);

                                if (spreadEvent.isCancelled()) {
                                    logger.debug("火焰向 {} 的蔓延被事件取消。", spreadPos);
                                    continue; // 跳过此次蔓延
                                }

                                auto fireBlockLegacyOpt = BlockLegacy::tryGetFromRegistry("minecraft:fire");
                                if (fireBlockLegacyOpt.has_value()) {
                                    region.setBlock(
                                        spreadPos,
                                        *fireBlockLegacyOpt.value().mDefaultState,
                                        3,
                                        nullptr,
                                        nullptr
                                    );
                                    logger.debug("火焰跳跃蔓延至 {} (概率: {})。", spreadPos, spreadChance);

                                    // --- 事件触发：火焰蔓延后 ---
                                    auto spreadAfterEvent =
                                        CauldronZero::event::FireSpreadAfterEvent(region, pos, spreadPos);
                                    ll::event::EventBus::getInstance().publish(spreadAfterEvent);
                                }
                            }
                        }
                    }
                }
            }
        }

        // --- 火焰存在条件检查 ---
        // 如果火焰下方不是固体方块，或者位置本身不适合火焰存在，则熄灭火焰
        if (!FireBlockEventHook::isSolidToppedBlock(region, belowPos) && !this->isValidFireLocation(region, pos)) {
            region.removeBlock(pos);
            logger.debug("火焰 @ {} 因下方无固体方块而熄灭。", pos);
            return;
        }

        // --- 火焰年龄增长与熄灭逻辑 ---
        if (stateOpt.has_value()) {
            int newAge = currentAge;

            // 非永恒火焰且年龄未满15时，有 50% 的概率年龄 +1
            if (!isEternalFire && currentAge < 15) {
                if (random.nextInt(100) < 50) {
                    newAge = currentAge + 1;
                }
            }
            // 永恒火焰年龄不变
            // 年龄达到15的非永恒火焰，年龄也不再增加

            // 当火焰年龄达到最大值 15 时，进行熄灭检定
            if (newAge >= 15) {
                // 根据下方方块的燃烧潜力决定熄灭概率
                float fireOdds = this->getFireOdds(region, belowPos);
                int extinguishChance = (fireOdds == 0.0f) ? 3 : 6; // 潜力为0，1/3概率熄灭；否则1/6概率熄灭

                if (random.nextInt(extinguishChance) == 0) {
                    region.removeBlock(pos);
                    logger.debug("火焰 @ {} 因老化而熄灭。", pos);
                    return;
                } else {
                    // 如果没有熄灭，则将年龄重置为15，防止溢出
                    newAge = 15;
                }
            }

            // 应用新的年龄状态到火焰方块
            auto newFireBlockOpt = currentFireBlock.setState<int>(1, newAge);
            if (newFireBlockOpt.has_value()) {
                region.setBlock(pos, newFireBlockOpt.value(), 3, nullptr, nullptr);
                // 为方块计划下一次 tick，使其能够持续更新
                this->_tryAddToTickingQueue(region, pos, random);
            } else {
                logger.debug("为火焰 @ {} 设置新年龄失败。", pos);
            }
        } else {
            logger.debug("无法获取火焰 @ {} 的年龄状态。", pos);
        }
    } catch (const SEH_Exception& e) {
        logger.error(
            "SEH 异常在 FireBlockEventHook::hook 中捕获: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(eventData); // 返回原始行为
        throw;             // 重新抛出异常
    } catch (const std::exception& e) {
        logger.error("标准异常在 FireBlockEventHook::hook 中捕获: {}", e.what());
        origin(eventData); // 返回原始行为
        throw;             // 重新抛出异常
    } catch (...) {
        logger.error("未知异常在 FireBlockEventHook::hook 中捕获");
        origin(eventData); // 返回原始行为
        throw;             // 重新抛出异常
    }
}
void registerFireBlockEventHooks() { FireBlockEventHook::hook(); }

void unregisterFireBlockEventHooks() { FireBlockEventHook::unhook(); }
} // namespace CauldronZero::event
