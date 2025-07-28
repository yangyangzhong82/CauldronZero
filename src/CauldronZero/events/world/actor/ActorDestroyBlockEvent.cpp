#include "CauldronZero/events/world/actor/ActorDestroyBlockEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/deps/ecs/gamerefs_entity/EntityContext.h"
#include "mc/deps/game_refs/WeakRef.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/events/ActorEventCoordinator.h"
#include "mc/world/events/ActorGameplayEvent.h"
#include "mc/world/events/ActorGriefingBlockEvent.h"
#include "mc/world/events/details/ValueOrRef.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/block/Block.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>

namespace CauldronZero::event {


Actor&       ActorDestroyBlockBeforeEvent::getActor() const { return mActor; }
Block const& ActorDestroyBlockBeforeEvent::getBlock() const { return mBlock; }
Vec3 const&  ActorDestroyBlockBeforeEvent::getPos() const { return mPos; }

// Emitter 负责在程序启动时将事件注册到 EventBus 中。
class ActorDestroyBlockBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorDestroyBlockBeforeEvent> {};

// 实现定义在 .h 文件中的 getter 方法
Actor&       ActorDestroyBlockAfterEvent::getActor() const { return mActor; }
Block const& ActorDestroyBlockAfterEvent::getBlock() const { return mBlock; }
Vec3 const&  ActorDestroyBlockAfterEvent::getPos() const { return mPos; }

// Emitter 负责在程序启动时将事件注册到 EventBus 中。
class ActorDestroyBlockAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::ActorDestroyBlockAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    ActorDestroyBlockHook,
    ll::memory::HookPriority::Normal,
    ::ActorEventCoordinator,
    &::ActorEventCoordinator::sendEvent,
    ::CoordinatorResult,
    ::EventRef<::ActorGameplayEvent<::CoordinatorResult>> const& event
) {
    try {
        const ActorGriefingBlockEvent* griefingEvent = nullptr;
        event.get().visit([&](auto&& arg) {
            using CurrentEventType = std::decay_t<decltype(arg.value())>;
            if constexpr (std::is_same_v<CurrentEventType, ActorGriefingBlockEvent>) {
                griefingEvent = &arg.value();
            }
        });

        if (griefingEvent) {
            // 获取 ActorContext 的 WeakRef
            WeakRef<::EntityContext> actorContextWeakRef = griefingEvent->mActorContext;
            // 锁定 WeakRef 获取 StackRefResult
            StackRefResult<::EntityContext> actorContextResult = actorContextWeakRef.lock();

            if (actorContextResult->isValid()) {
                // 获取 EntityContext 引用
                ::EntityContext& entityContext = actorContextResult.value();
                // 使用 Actor::tryGetFromEntity 获取 Actor 指针
                ::Actor* actor = Actor::tryGetFromEntity(entityContext, false);

                if (actor) {
                    // 创建并发布 ActorDestroyBlockBeforeEvent
                    auto beforeEvent = CauldronZero::event::ActorDestroyBlockBeforeEvent(
                        *actor,
                        *griefingEvent->mBlock,
                        griefingEvent->mPos
                    );
                    ll::event::EventBus::getInstance().publish(beforeEvent);

                    if (beforeEvent.isCancelled()) {
                        logger.debug("ActorDestroyBlockBeforeEvent cancelled!");
                        return CoordinatorResult::Cancel;
                    }

                    // 调用原始函数
                    CoordinatorResult result = origin(event);

                    // 发布 ActorDestroyBlockAfterEvent
                    auto afterEvent = CauldronZero::event::ActorDestroyBlockAfterEvent(
                        *actor,
                        *griefingEvent->mBlock,
                        griefingEvent->mPos
                    );
                    ll::event::EventBus::getInstance().publish(afterEvent);

                    return result;
                }
            }
        }
        return origin(event);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "ActorDestroyBlockHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        return origin(event);
    } catch (const std::exception& e) {
        logger.warn("ActorDestroyBlockHook 发生 C++ 异常: {}", e.what());
        return origin(event);
    } catch (...) {
        logger.warn("ActorDestroyBlockHook 发生未知异常！");
        return origin(event);
    }
}
void registerActorDestroyBlockHooks() { ActorDestroyBlockHook::hook(); }

void unregisterActorDestroyBlockHooks() { ActorDestroyBlockHook::unhook(); }
} // namespace CauldronZero::event
