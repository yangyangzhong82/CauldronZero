#pragma once

#include "CauldronZero/events/world/block/FireSpreadEvent.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/level/BlockSource.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>

namespace CauldronZero::event {

// --- FireSpreadBeforeEvent ---
BlockSource&    FireSpreadBeforeEvent::getRegion() const { return mRegion; }
const BlockPos& FireSpreadBeforeEvent::getFirePos() const { return mFirePos; }
const BlockPos& FireSpreadBeforeEvent::getSpreadPos() const { return mSpreadPos; }

void FireSpreadBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["dimension"] = static_cast<int>(mRegion.getDimensionId());
    nbt["firePos"]   = ListTag{mFirePos.x, mFirePos.y, mFirePos.z};
    nbt["spreadPos"] = ListTag{mSpreadPos.x, mSpreadPos.y, mSpreadPos.z};
}

class FireSpreadBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::FireSpreadBeforeEvent> {};


// --- FireSpreadAfterEvent ---
BlockSource&    FireSpreadAfterEvent::getRegion() const { return mRegion; }
const BlockPos& FireSpreadAfterEvent::getFirePos() const { return mFirePos; }
const BlockPos& FireSpreadAfterEvent::getSpreadPos() const { return mSpreadPos; }

void FireSpreadAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["dimension"] = static_cast<int>(mRegion.getDimensionId());
    nbt["firePos"]   = ListTag{mFirePos.x, mFirePos.y, mFirePos.z};
    nbt["spreadPos"] = ListTag{mSpreadPos.x, mSpreadPos.y, mSpreadPos.z};
}

class FireSpreadAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::FireSpreadAfterEvent> {};

} // namespace CauldronZero::event
