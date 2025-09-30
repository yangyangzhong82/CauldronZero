#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/attribute/AttributeBuff.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Mob;

namespace CauldronZero::event {

// --- MobHealthChangeBeforeEvent ---
// 此事件在生物生命值更改之前触发。
// 它是可取消的，用以阻止该行为。
class MobHealthChangeBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Mob&                     mMob;
    float                    mOldValue;
    float                    mNewValue;
    ::AttributeBuff const&   mBuff;

public:
    constexpr explicit MobHealthChangeBeforeEvent(
        Mob&                     mob,
        float                    oldValue,
        float                    newValue,
        ::AttributeBuff const&   buff
    )
    : mMob(mob),
      mOldValue(oldValue),
      mNewValue(newValue),
      mBuff(buff) {}

    CZ_API Mob&                     getMob() const;
    CZ_API float                    getOldValue() const;
    CZ_API float                    getNewValue() const;
    CZ_API ::AttributeBuff const&   getBuff() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- MobHealthChangeAfterEvent ---
// 此事件在生物生命值更改之后触发。
class MobHealthChangeAfterEvent final : public ll::event::Event {
protected:
    Mob&                     mMob;
    float                    mOldValue;
    float                    mPreChangeValue; // 更改前的值
    float                    mNewValue;       // 更改后的值 (origin 的返回值)
    ::AttributeBuff const&   mBuff;

public:
    constexpr explicit MobHealthChangeAfterEvent(
        Mob&                     mob,
        float                    oldValue,
        float                    preChangeValue,
        float                    newValue,
        ::AttributeBuff const&   buff
    )
    : mMob(mob),
      mOldValue(oldValue),
      mPreChangeValue(preChangeValue),
      mNewValue(newValue),
      mBuff(buff) {}

    CZ_API Mob&                     getMob() const;
    CZ_API float                    getOldValue() const;
    CZ_API float                    getPreChangeValue() const;
    CZ_API float                    getNewValue() const;
    CZ_API ::AttributeBuff const&   getBuff() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerMobHealthChangeEventHooks();
void unregisterMobHealthChangeEventHooks();

} // namespace CauldronZero::event
