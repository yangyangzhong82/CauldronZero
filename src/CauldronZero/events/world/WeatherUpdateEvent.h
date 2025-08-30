#pragma once

#include "CauldronZero/Macros.h"
#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventRefObjSerializer.h>

class Level;

namespace CauldronZero::event {

// --- WeatherUpdateBeforeEvent ---
// 此事件在天气更新之前触发。
// 它是可取消的，用以阻止该行为。
class WeatherUpdateBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
protected:
    Level& mLevel;
    float  mRainLevel;
    int    mRainTime;
    float  mLightningLevel;
    int    mLightningTime;

public:
    constexpr explicit WeatherUpdateBeforeEvent(
        Level& level,
        float  rainLevel,
        int    rainTime,
        float  lightningLevel,
        int    lightningTime
    )
    : mLevel(level),
      mRainLevel(rainLevel),
      mRainTime(rainTime),
      mLightningLevel(lightningLevel),
      mLightningTime(lightningTime) {}

    CZ_API Level& getLevel() const;
    CZ_API float  getRainLevel() const;
    CZ_API int    getRainTime() const;
    CZ_API float  getLightningLevel() const;
    CZ_API int    getLightningTime() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

// --- WeatherUpdateAfterEvent ---
// 此事件在天气更新之后触发。
class WeatherUpdateAfterEvent final : public ll::event::Event {
protected:
    Level& mLevel;
    float  mRainLevel;
    int    mRainTime;
    float  mLightningLevel;
    int    mLightningTime;

public:
    constexpr explicit WeatherUpdateAfterEvent(
        Level& level,
        float  rainLevel,
        int    rainTime,
        float  lightningLevel,
        int    lightningTime
    )
    : mLevel(level),
      mRainLevel(rainLevel),
      mRainTime(rainTime),
      mLightningLevel(lightningLevel),
      mLightningTime(lightningTime) {}

    CZ_API Level& getLevel() const;
    CZ_API float  getRainLevel() const;
    CZ_API int    getRainTime() const;
    CZ_API float  getLightningLevel() const;
    CZ_API int    getLightningTime() const;

    virtual void serialize(CompoundTag& nbt) const override;
};

void registerWeatherUpdateEventHooks();
void unregisterWeatherUpdateEventHooks();

} // namespace CauldronZero::event
