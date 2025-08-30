#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Actor;
class MobEffectInstance;

namespace CauldronZero::event {

// --- ActorEffectAddBeforeEvent ---
// 此事件在生物获得状态效果之前触发。
// 它是可取消的，用以阻止该行为。
class ActorEffectAddBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Actor&             mActor;
    MobEffectInstance& mEffect;

public:
    constexpr explicit ActorEffectAddBeforeEvent(Actor& actor, MobEffectInstance& effect)
    : mActor(actor),
      mEffect(effect) {}

    CZ_API Actor&             getActor() const;
    CZ_API MobEffectInstance& getEffect() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- ActorEffectAddAfterEvent ---
// 此事件在生物获得状态效果之后触发。
class ActorEffectAddAfterEvent final : public ll::event::Event {
protected:
    Actor&             mActor;
    MobEffectInstance& mEffect;

public:
    constexpr explicit ActorEffectAddAfterEvent(Actor& actor, MobEffectInstance& effect)
    : mActor(actor),
      mEffect(effect) {}

    CZ_API Actor&             getActor() const;
    CZ_API MobEffectInstance& getEffect() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- ActorEffectRemoveBeforeEvent ---
// 此事件在生物移除状态效果之前触发。
// 它是可取消的，用以阻止该行为。
class ActorEffectRemoveBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Actor&             mActor;
    MobEffectInstance& mEffect;

public:
    constexpr explicit ActorEffectRemoveBeforeEvent(Actor& actor, MobEffectInstance& effect)
    : mActor(actor),
      mEffect(effect) {}

    CZ_API Actor&             getActor() const;
    CZ_API MobEffectInstance& getEffect() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- ActorEffectRemoveAfterEvent ---
// 此事件在生物移除状态效果之后触发。
class ActorEffectRemoveAfterEvent final : public ll::event::Event {
protected:
    Actor&             mActor;
    MobEffectInstance& mEffect;

public:
    constexpr explicit ActorEffectRemoveAfterEvent(Actor& actor, MobEffectInstance& effect)
    : mActor(actor),
      mEffect(effect) {}

    CZ_API Actor&             getActor() const;
    CZ_API MobEffectInstance& getEffect() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- ActorEffectUpdateBeforeEvent ---
// 此事件在生物状态效果更新之前触发。
// 它是可取消的，用以阻止该行为。
class ActorEffectUpdateBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Actor&             mActor;
    MobEffectInstance& mEffect;

public:
    constexpr explicit ActorEffectUpdateBeforeEvent(Actor& actor, MobEffectInstance& effect)
    : mActor(actor),
      mEffect(effect) {}

    CZ_API Actor&             getActor() const;
    CZ_API MobEffectInstance& getEffect() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- ActorEffectUpdateAfterEvent ---
// 此事件在生物状态效果更新之后触发。
class ActorEffectUpdateAfterEvent final : public ll::event::Event {
protected:
    Actor&             mActor;
    MobEffectInstance& mEffect;

public:
    constexpr explicit ActorEffectUpdateAfterEvent(Actor& actor, MobEffectInstance& effect)
    : mActor(actor),
      mEffect(effect) {}

    CZ_API Actor&             getActor() const;
    CZ_API MobEffectInstance& getEffect() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerActorEffectUpdateEventHooks();
void unregisterActorEffectUpdateEventHooks();

} // namespace CauldronZero::event
