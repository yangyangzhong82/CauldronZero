#include "CauldronZero/events/world/block/RedstoneUpdateEvent.h"
#include "CauldronZero/Logger.h"
#include "CauldronZero/SEHHandler.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "ll/api/memory/Hook.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/redstone/circuit/CircuitSystem.h"

#include "mc/world/redstone/circuit/ChunkCircuitComponentList.h"
#include "mc/world/level/block/actor/BlockActor.h"
#include "mc/world/redstone/circuit/CircuitSceneGraph.h"
#include "mc/world/redstone/circuit/components/BaseCircuitComponent.h"


namespace CauldronZero::event {

// --- RedstoneUpdateBeforeEvent ---
BlockSource&         RedstoneUpdateBeforeEvent::getRegion() const { return mRegion; }
const BlockPos&      RedstoneUpdateBeforeEvent::getPos() const { return mPos; }
int                  RedstoneUpdateBeforeEvent::getNewStrength() const { return mNewStrength; }
bool                 RedstoneUpdateBeforeEvent::isFirstTime() const { return mIsFirstTime; }
BaseCircuitComponent* RedstoneUpdateBeforeEvent::getComponent() const { return mComponent; }

void RedstoneUpdateBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["dimension"]    = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]          = ListTag{mPos.x, mPos.y, mPos.z};
    nbt["newStrength"]  = mNewStrength;
    nbt["isFirstTime"]  = mIsFirstTime;
    // mComponent 无法直接序列化
}

class RedstoneUpdateBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, RedstoneUpdateBeforeEvent> {};

// --- RedstoneUpdateAfterEvent ---
BlockSource&         RedstoneUpdateAfterEvent::getRegion() const { return mRegion; }
const BlockPos&      RedstoneUpdateAfterEvent::getPos() const { return mPos; }
int                  RedstoneUpdateAfterEvent::getNewStrength() const { return mNewStrength; }
bool                 RedstoneUpdateAfterEvent::isFirstTime() const { return mIsFirstTime; }
BaseCircuitComponent* RedstoneUpdateAfterEvent::getComponent() const { return mComponent; }

void RedstoneUpdateAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["dimension"]    = static_cast<int>(mRegion.getDimensionId());
    nbt["pos"]          = ListTag{mPos.x, mPos.y, mPos.z};
    nbt["newStrength"]  = mNewStrength;
    nbt["isFirstTime"]  = mIsFirstTime;
    // mComponent 无法直接序列化
}

class RedstoneUpdateAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, RedstoneUpdateAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    RedstoneUpdateEventHook, // Changed hook name for clarity
    ll::memory::HookPriority::Normal,
    CircuitSystem,
    &CircuitSystem::updateBlocks,
    void,
    ::BlockSource&    region,
    ::BlockPos const& chunkPos
) {
    try {
        if (this->mHasBeenEvaluated) {
            auto& componentsByChunk  = this->mSceneGraph->mActiveComponentsPerChunk;
            auto  chunkEntryIterator = componentsByChunk.find(chunkPos);

            if (chunkEntryIterator != componentsByChunk.end()) {
                std::vector<ChunkCircuitComponentList::Item> secondaryUpdateQueue;
                ChunkCircuitComponentList& chunkComponentList = chunkEntryIterator->second;

                for (const auto& listItem : *chunkComponentList.mComponents) {
                    BaseCircuitComponent* component = listItem.mComponent;
                    if (!component) continue;

                    if (component->mNeedsUpdate && !component->mRemoved) {
                        component->mNeedsUpdate = false;

                        if (component->isSecondaryPowered()) {
                            secondaryUpdateQueue.push_back(listItem);
                        } else {
                            int newStrength = component->getStrength();
                            if (newStrength != -1) {
                                auto beforeEvent = RedstoneUpdateBeforeEvent(
                                    region,
                                    component->mPos,
                                    newStrength,
                                    component->mIsFirstTime,
                                    component
                                );
                                ll::event::EventBus::getInstance().publish(beforeEvent);

                                if (!beforeEvent.isCancelled()) {
                                    if (!component->mIsFirstTime || !component->mIgnoreFirstUpdate) {
                                        const Block& blockToUpdate = region.getBlock(component->mPos);
                                        blockToUpdate.getLegacyBlock().onRedstoneUpdate(
                                            region,
                                            component->mPos,
                                            newStrength,
                                            component->mIsFirstTime
                                        );
                                    }
                                    auto afterEvent = RedstoneUpdateAfterEvent(
                                        region,
                                        component->mPos,
                                        newStrength,
                                        component->mIsFirstTime,
                                        component
                                    );
                                    ll::event::EventBus::getInstance().publish(afterEvent);
                                }
                            }
                            component->mIsFirstTime = false;
                        }
                    }
                }

                for (const auto& listItem : secondaryUpdateQueue) {
                    BaseCircuitComponent* component = listItem.mComponent;
                    if (!component) continue;

                    int newStrength = component->getStrength();
                    if (newStrength != -1) {
                        auto beforeEvent = RedstoneUpdateBeforeEvent(
                            region,
                            component->mPos,
                            newStrength,
                            component->mIsFirstTime,
                            component
                        );
                        ll::event::EventBus::getInstance().publish(beforeEvent);

                        if (!beforeEvent.isCancelled()) {
                            if (!component->mIsFirstTime || !component->mIgnoreFirstUpdate) {
                                const Block& blockToUpdate = region.getBlock(component->mPos);
                                blockToUpdate.getLegacyBlock().onRedstoneUpdate(
                                    region,
                                    component->mPos,
                                    newStrength,
                                    component->mIsFirstTime
                                );
                            }
                            auto afterEvent = RedstoneUpdateAfterEvent(
                                region,
                                component->mPos,
                                newStrength,
                                component->mIsFirstTime,
                                component
                            );
                            ll::event::EventBus::getInstance().publish(afterEvent);
                        }
                    }
                    component->mIsFirstTime = false;
                }
            }
        }
    } catch (const SEH_Exception& e) {
        logger.error(
            "在 RedstoneUpdateEventHook 中捕获到 SEH 异常: Code = 0x{:x}, Address = {}, What = {}",
            e.getSehCode(),
            e.getExceptionAddress(),
            e.what()
        );
        origin(region, chunkPos);
        throw;
    } catch (const std::exception& e) {
        logger.error("在 RedstoneUpdateEventHook 中捕获到标准异常: {}", e.what());
        origin(region, chunkPos);
        throw;
    } catch (...) {
        logger.error("在 RedstoneUpdateEventHook 中捕获到未知异常");
        origin(region, chunkPos);
        throw;
    }
}

void registerRedstoneUpdateEventHooks() { RedstoneUpdateEventHook::hook(); }
void unregisterRedstoneUpdateEventHooks() { RedstoneUpdateEventHook::unhook(); }

} // namespace CauldronZero::event
