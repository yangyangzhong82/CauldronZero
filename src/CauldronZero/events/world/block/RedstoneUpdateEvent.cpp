#include "CauldronZero/Logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include "mc\world\redstone\circuit\CircuitSystem.h"

#include "mc/world/redstone/circuit/ChunkCircuitComponentList.h"
#include "mc\world\level\block\actor\BlockActor.h"
#include "mc\world\redstone\circuit/CircuitSceneGraph.h"
#include "mc\world\redstone\circuit\components\BaseCircuitComponent.h"


namespace CauldronZero::event {
LL_AUTO_TYPE_INSTANCE_HOOK(
    Hookda2,
    ll::memory::HookPriority::Normal,
    CircuitSystem,
    &CircuitSystem::updateBlocks,
    void,
    ::BlockSource&    region,
    ::BlockPos const& chunkPos
) {
    // 伪代码: if ( *((_BYTE *)this + 440) )
    // 对应: 检查 mHasBeenEvaluated 标志。
    if (this->mHasBeenEvaluated) {

        // 伪代码: v4 = (char *)this + 72;
        // 对应: 获取 mSceneGraph.mPowerAssociationMap 的引用
        auto& powerMap = this->mSceneGraph->mPowerAssociationMap;

        // 伪代码: if ( *((_QWORD *)this + 11) )
        // 对应: 检查 mPowerAssociationMap 是否为空 (或者某个内部状态)。
        // 现代C++中可以直接进行 find 操作。

        // 伪代码: std::_Hash<...>::find<...>(... , a3);
        // 对应: 在 powerMap 中查找传入的 pos。
        auto mapEntryIterator = powerMap.find(chunkPos);

        // 1. 使用正确的 Map
        auto& componentsByChunk  = this->mSceneGraph->mActiveComponentsPerChunk;
        auto  chunkEntryIterator = componentsByChunk.find(chunkPos);

        // 2. 检查是否找到了该区块的元件列表
        if (chunkEntryIterator != componentsByChunk.end()) {

            // 伪代码: v19 = 0LL; v20 = 0LL;
            // 对应: 初始化一个 std::vector 用于存储需要延迟更新的次级元件。
            std::vector<ChunkCircuitComponentList::Item> secondaryUpdateQueue;

            // --- 阶段一：遍历找到的元件列表 ---
            // 伪代码: v7 = *(_QWORD *)(v21 + 40); ... do-while ...
            // 对应: 遍历 mapEntryIterator->second (即 CircuitComponentList) 内部的 mComponents。
            ChunkCircuitComponentList& chunkComponentList = chunkEntryIterator->second;

            // 3. 遍历区块内的所有元件
            for (const auto& listItem : *chunkComponentList.mComponents) {

                BaseCircuitComponent* component = listItem.mComponent;

                // 伪代码: if ( !*(_QWORD *)v7 ) gsl::details::terminate(v5);
                // 这是一个非空断言。
                if (!component) continue;

                // 伪代码: if ( v8[50] == 1 && !v8[87] )
                // 对应: 检查 mNeedsUpdate 和 mRemoved 标志。
                if (component->mNeedsUpdate && !component->mRemoved) {

                    // 伪代码: v8[50] = 0;
                    // 清除更新标记。
                    component->mNeedsUpdate = false;

                    // 伪代码: (*( ... ))(*(_QWORD *)v8 + 176LL))(v8)
                    // 对应: 调用 isSecondaryPowered()。
                    if (component->isSecondaryPowered()) {
                        // 如果是次级元件，加入延迟队列。
                        // 伪代码中的 sub_140182880 就是 vector::push_back (或 insert) 的实现。
                        secondaryUpdateQueue.push_back(listItem);
                    } else {
                        // 如果是主电源，立即更新。
                        int newStrength = component->getStrength();
                        if (newStrength != -1) {
                            // 伪代码: if ( !v8[49] || !v8[48] )
                            // 对应: 检查是否需要跳过首次更新。
                            if (!component->mIsFirstTime || !component->mIgnoreFirstUpdate) {
                                const Block& blockToUpdate = region.getBlock(component->mPos);
                                // 伪代码: (*(void (__fastcall **)(...))(*(_QWORD *)v5 + 488LL))(...);
                                // 对应: 调用 BlockLegacy::onRedstoneUpdate。
                                blockToUpdate.getLegacyBlock()
                                    .onRedstoneUpdate(region, component->mPos, newStrength, component->mIsFirstTime);
                            }
                        }
                        // 伪代码: v8[49] = 0;
                        // 重置首次更新标志。
                        component->mIsFirstTime = false;
                    }
                }
            } // 阶段一循环结束

            // --- 阶段二：处理所有被延迟的次级元件 ---
            // 伪代码: v12 = (char *)v19; ... do-while ...
            // 对应: 遍历 secondaryUpdateQueue。
            for (const auto& listItem : secondaryUpdateQueue) {
                BaseCircuitComponent* component = listItem.mComponent;
                if (!component) continue;

                // 这部分逻辑与上面主电源的更新逻辑完全相同。
                int newStrength = component->getStrength();
                if (newStrength != -1) {
                    if (!component->mIsFirstTime || !component->mIgnoreFirstUpdate) {
                        const Block& blockToUpdate = region.getBlock(component->mPos);
                        blockToUpdate.getLegacyBlock()
                            .onRedstoneUpdate(region, component->mPos, newStrength, component->mIsFirstTime);
                    }
                }
                component->mIsFirstTime = false;
            } // 阶段二循环结束
        }
    }
}
} // namespace CauldronZero::event
