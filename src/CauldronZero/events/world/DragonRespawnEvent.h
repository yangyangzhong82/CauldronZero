#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class EndDragonFight;
enum class RespawnAnimation; // 声明 RespawnAnimation 枚举

namespace CauldronZero::event {

// --- DragonRespawnBeforeEvent ---
// 此事件在末影龙尝试复活之前触发。
// 它是可取消的，用以阻止该行为。
class DragonRespawnBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    EndDragonFight&    mEndDragonFight;
    RespawnAnimation&  mStage;

public:
    constexpr explicit DragonRespawnBeforeEvent(EndDragonFight& endDragonFight, RespawnAnimation& stage)
    : mEndDragonFight(endDragonFight),
      mStage(stage) {}

    CZ_API EndDragonFight&   getEndDragonFight() const;
    CZ_API RespawnAnimation& getStage() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- DragonRespawnAfterEvent ---
// 此事件在末影龙完成复活之后触发。
class DragonRespawnAfterEvent final : public ll::event::Event {
protected:
    EndDragonFight&    mEndDragonFight;
    RespawnAnimation&  mStage;

public:
    constexpr explicit DragonRespawnAfterEvent(EndDragonFight& endDragonFight, RespawnAnimation& stage)
    : mEndDragonFight(endDragonFight),
      mStage(stage) {}

    CZ_API EndDragonFight&   getEndDragonFight() const;
    CZ_API RespawnAnimation& getStage() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerDragonRespawnEventHooks();
void unregisterDragonRespawnEventHooks();

} // namespace CauldronZero::event
