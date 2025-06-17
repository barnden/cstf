#pragma once

#include "Types.h"
#include <format>

namespace CSTF {

#pragma pack(1)
struct _PlayerMoveEventData {
    u8 padding {};
    u32 yaw : 24 {};
    u32 pitch : 24 {};
    u8 player {};
};
#pragma pack()

struct PlayerMoveEvent {
    _PlayerMoveEventData data {};
    float position[3] {};

    [[nodiscard]] constexpr auto pitch() const noexcept -> float
    {
        return static_cast<float>(data.pitch) / ((1ul << 24) - 1) * 180.;
    }

    [[nodiscard]] constexpr auto yaw() const noexcept -> float
    {
        return static_cast<float>(data.yaw) / ((1ul << 24) - 1) * 360.;
    }

    auto to_string() const -> std::string
    {
        return std::format("PlayerMoveEvent(player: {}, pitch: {:0.3f}, yaw: {:0.3f}, position: {})",
                           data.player,
                           pitch(),
                           yaw(),
                           position);
    }
};

}