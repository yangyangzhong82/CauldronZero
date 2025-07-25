#pragma once

#include "mc/nbt/CompoundTag.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>

#include "CauldronZero/Macros.h"
class BlockSource;
class BlockPos ;
namespace CauldronZero::event {

    class FireSpreadBeforeEvent final : public ll::event::Cancellable<ll::event::Event> {
    protected:
        BlockSource&    mRegion;    // 事件发生的区域。
        const BlockPos& mFirePos;   // 原始火焰方块的位置。
        const BlockPos& mSpreadPos; // 火焰将要蔓延到的新位置。

    public:
        constexpr explicit FireSpreadBeforeEvent(
            BlockSource&    region,
            const BlockPos& firePos,
            const BlockPos& spreadPos
        )
        : mRegion(region),
          mFirePos(firePos),
          mSpreadPos(spreadPos) {}

        // 获取事件发生的区域。
        CZ_API BlockSource& getRegion() const;

        // 获取原始火焰方块的位置。
        CZ_API const BlockPos& getFirePos() const;


        // 获取火焰将要蔓延到的新位置。
        CZ_API const BlockPos& getSpreadPos() const;

        CZ_API virtual void serialize(CompoundTag& nbt) const override;
    };


    // 此事件在火焰成功蔓延到一个新位置之后触发。
    // 此事件不可取消。
    class FireSpreadAfterEvent final : public ll::event::Event {
    protected:
        BlockSource&    mRegion;    // 事件发生的区域。
        const BlockPos& mFirePos;   // 原始火焰方块的位置。
        const BlockPos& mSpreadPos; // 火焰蔓延到的新位置。

    public:
        constexpr explicit FireSpreadAfterEvent(BlockSource& region, const BlockPos& firePos, const BlockPos& spreadPos)
        : mRegion(region),
          mFirePos(firePos),
          mSpreadPos(spreadPos) {}

        // 获取事件发生的区域。
        CZ_API BlockSource& getRegion() const;

        // 获取原始火焰方块的位置。
        CZ_API const BlockPos& getFirePos() const;

        // 获取火焰蔓延到的新位置。
        CZ_API const BlockPos& getSpreadPos() const;

        virtual void serialize(CompoundTag& nbt) const override;
    };
    void registerFireBlockEventHooks();

    void unregisterFireBlockEventHooks();
} // namespace CauldronZero::event
