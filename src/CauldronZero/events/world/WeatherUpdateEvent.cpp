#include "WeatherUpdateEvent.h"
#include "CauldronZero/SEHHandler.h"
#include "CauldronZero/logger.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/Level.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/EventRefObjSerializer.h>

namespace CauldronZero::event {

// --- WeatherUpdateBeforeEvent ---
Level& WeatherUpdateBeforeEvent::getLevel() const { return mLevel; }
float  WeatherUpdateBeforeEvent::getRainLevel() const { return mRainLevel; }
int    WeatherUpdateBeforeEvent::getRainTime() const { return mRainTime; }
float  WeatherUpdateBeforeEvent::getLightningLevel() const { return mLightningLevel; }
int    WeatherUpdateBeforeEvent::getLightningTime() const { return mLightningTime; }

void WeatherUpdateBeforeEvent::serialize(CompoundTag& nbt) const {
    Cancellable::serialize(nbt);
    nbt["level"]          = ll::event::serializeRefObj(mLevel);
    nbt["rainLevel"]      = mRainLevel;
    nbt["rainTime"]       = mRainTime;
    nbt["lightningLevel"] = mLightningLevel;
    nbt["lightningTime"]  = mLightningTime;
}

class WeatherUpdateBeforeEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::WeatherUpdateBeforeEvent> {};

// --- WeatherUpdateAfterEvent ---
Level& WeatherUpdateAfterEvent::getLevel() const { return mLevel; }
float  WeatherUpdateAfterEvent::getRainLevel() const { return mRainLevel; }
int    WeatherUpdateAfterEvent::getRainTime() const { return mRainTime; }
float  WeatherUpdateAfterEvent::getLightningLevel() const { return mLightningLevel; }
int    WeatherUpdateAfterEvent::getLightningTime() const { return mLightningTime; }

void WeatherUpdateAfterEvent::serialize(CompoundTag& nbt) const {
    Event::serialize(nbt);
    nbt["level"]          = ll::event::serializeRefObj(mLevel);
    nbt["rainLevel"]      = mRainLevel;
    nbt["rainTime"]       = mRainTime;
    nbt["lightningLevel"] = mLightningLevel;
    nbt["lightningTime"]  = mLightningTime;
}

class WeatherUpdateAfterEventEmitter
: public ll::event::Emitter<[](auto&&...) { return nullptr; }, CauldronZero::event::WeatherUpdateAfterEvent> {};


LL_TYPE_INSTANCE_HOOK(
    WeatherUpdateHook,
    HookPriority::Normal,
    Level,
    &Level::$updateWeather,
    void,
    float rainLevel,
    int   rainTime,
    float lightningLevel,
    int   lightningTime
) {
    try {
        auto& level       = *this;
        auto  beforeEvent = WeatherUpdateBeforeEvent(level, rainLevel, rainTime, lightningLevel, lightningTime);
        ll::event::EventBus::getInstance().publish(beforeEvent);

        if (beforeEvent.isCancelled()) {
            return;
        }

        origin(rainLevel, rainTime, lightningLevel, lightningTime);

        auto afterEvent = WeatherUpdateAfterEvent(level, rainLevel, rainTime, lightningLevel, lightningTime);
        ll::event::EventBus::getInstance().publish(afterEvent);
    } catch (const SEH_Exception& e) {
        logger.warn(
            "WeatherUpdateHook 发生 SEH 异常: 代码 {}, 地址 {:#x}, 描述: {}",
            e.getSehCode(),
            (uintptr_t)e.getExceptionAddress(),
            e.what()
        );
        origin(rainLevel, rainTime, lightningLevel, lightningTime);
    } catch (const std::exception& e) {
        logger.warn("WeatherUpdateHook 发生 C++ 异常: {}", e.what());
        origin(rainLevel, rainTime, lightningLevel, lightningTime);
    } catch (...) {
        logger.warn("WeatherUpdateHook 发生未知异常！");
        origin(rainLevel, rainTime, lightningLevel, lightningTime);
    }
}

void registerWeatherUpdateEventHooks() { WeatherUpdateHook::hook(); }
void unregisterWeatherUpdateEventHooks() { WeatherUpdateHook::unhook(); }

} // namespace CauldronZero::event
