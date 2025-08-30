#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

// Forward declarations
class BlockSource;
class BlockPos;
class Block;
class BaseCircuitComponent;

namespace CauldronZero::event {

// 此事件在红石元件更新之前触发。
// 它是可取消的。
class RedstoneUpdateBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    BlockSource&         mRegion;
    const BlockPos&      mPos;
    int                  mNewStrength;
    bool                 mIsFirstTime;
    BaseCircuitComponent* mComponent;

public:
    constexpr explicit RedstoneUpdateBeforeEvent(
        BlockSource&         region,
        const BlockPos&      pos,
        int                  newStrength,
        bool                 isFirstTime,
        BaseCircuitComponent* component
    )
    : mRegion(region),
      mPos(pos),
      mNewStrength(newStrength),
      mIsFirstTime(isFirstTime),
      mComponent(component) {}

    CZ_API BlockSource&         getRegion() const;
    CZ_API const BlockPos&      getPos() const;
    CZ_API int                  getNewStrength() const;
    CZ_API bool                 isFirstTime() const;
    CZ_API BaseCircuitComponent* getComponent() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// 此事件在红石元件更新之后触发。
// 此事件不可取消。
class RedstoneUpdateAfterEvent final : public ll::event::Event {
protected:
    BlockSource&         mRegion;
    const BlockPos&      mPos;
    int                  mNewStrength;
    bool                 mIsFirstTime;
    BaseCircuitComponent* mComponent;

public:
    constexpr explicit RedstoneUpdateAfterEvent(
        BlockSource&         region,
        const BlockPos&      pos,
        int                  newStrength,
        bool                 isFirstTime,
        BaseCircuitComponent* component
    )
    : mRegion(region),
      mPos(pos),
      mNewStrength(newStrength),
      mIsFirstTime(isFirstTime),
      mComponent(component) {}

    CZ_API BlockSource&         getRegion() const;
    CZ_API const BlockPos&      getPos() const;
    CZ_API int                  getNewStrength() const;
    CZ_API bool                 isFirstTime() const;
    CZ_API BaseCircuitComponent* getComponent() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerRedstoneUpdateEventHooks();
void unregisterRedstoneUpdateEventHooks();

} // namespace CauldronZero::event
