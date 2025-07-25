#pragma once

#include "CauldronZero/Macros.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/EventRefObjSerializer.h>

class Explosion;

namespace CauldronZero::event {

// 此事件在爆炸发生之前触发。
// 它是可取消的，用以阻止该行为。
class ExplosionBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Explosion& mExplosion;

public:
    constexpr explicit ExplosionBeforeEvent(Explosion& explosion)
    : mExplosion(explosion) {}

    CZ_API Explosion& getExplosion() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在爆炸发生之后触发。
class ExplosionAfterEvent final : public ll::event::Event {
protected:
    Explosion& mExplosion;

public:
    constexpr explicit ExplosionAfterEvent(Explosion& explosion)
    : mExplosion(explosion) {}

    CZ_API Explosion& getExplosion() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerExplosionEventHooks();

void unregisterExplosionEventHooks();

} // namespace CauldronZero::event
