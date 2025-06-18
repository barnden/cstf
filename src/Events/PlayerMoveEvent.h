#pragma once

#include "BaseEvent.h"
#include "Types.h"
#include <format>

namespace CSTF {

struct PlayerMoveEvent : Event<PlayerMoveEvent> {
    struct Data : public ISerializable<Data> {
        // NOTE: Why do we get misalignment when casting char[80] to Data if we
        //       pack the entire struct and not these fields.
        //       sizeof(Data) is 20 in either case.
#pragma pack(1)
        u8 padding {};
        u32 yaw : 24 {};
        u32 pitch : 24 {};
        u8 player {};
#pragma pack()

        float3 position;
    } data {};

    PlayerMoveEvent() = default;

    [[nodiscard]] constexpr auto pitch() const noexcept -> float
    {
        return static_cast<float>(data.pitch) / ((1ul << 24) - 1) * 180.;
    }

    [[nodiscard]] constexpr auto yaw() const noexcept -> float
    {
        return static_cast<float>(data.yaw) / ((1ul << 24) - 1) * 360.;
    }

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format("PlayerMoveEvent(player: {}, pitch: {:0.3f}, yaw: {:0.3f}, position: [{:0.3f}, {:0.3f}, {:0.3f}])",
                           data.player,
                           pitch(),
                           yaw(),
                           data.position.x, data.position.y, data.position.z);
    }
};

}