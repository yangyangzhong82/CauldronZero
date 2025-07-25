#pragma once

#include "CauldronZero/events/world/block/FireBlockBurnEvent.h"
#include "ll/api/event/EventRefObjSerializer.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>

namespace CauldronZero::event {

// --- FireBlockBurnBeforeEvent ---
BlockSource&    FireBlockBurnBeforeEvent::getRegion() const { return mRegion; }
const BlockPos& FireBlockBurnBeforeEvent::getFirePos() const { return mFirePos; }
const BlockPos& FireBlockBurnBeforeEvent::getBurnedPos() const { return mBurnedPos; }
const Block&    FireBlockBurnBeforeEvent::getBurnedBlock() const { return mBurnedBlock; }

void FireBlockBurnBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["dimension"]   = static_cast<int>(mRegion.getDimensionId());
    nbt["firePos"]     = ListTag{mFirePos.x, mFirePos.y, mFirePos.z};
    nbt["burnedPos"]   = ListTag{mBurnedPos.x, mBurnedPos.y, mBurnedPos.z};
    nbt["burnedBlock"] = ll::event::serializeRefObj(mBurnedBlock);
}

class FireBlockBurnBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::FireBlockBurnBeforeEvent> {};


// --- FireBlockBurnAfterEvent ---
BlockSource&    FireBlockBurnAfterEvent::getRegion() const { return mRegion; }
const BlockPos& FireBlockBurnAfterEvent::getFirePos() const { return mFirePos; }
const BlockPos& FireBlockBurnAfterEvent::getBurnedPos() const { return mBurnedPos; }

void FireBlockBurnAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["dimension"] = static_cast<int>(mRegion.getDimensionId());
    nbt["firePos"]   = ListTag{mFirePos.x, mFirePos.y, mFirePos.z};
    nbt["burnedPos"] = ListTag{mBurnedPos.x, mBurnedPos.y, mBurnedPos.z};
}

class FireBlockBurnAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::FireBlockBurnAfterEvent> {};

} // namespace CauldronZero::event
