#pragma once

#include "BaseEvent.h"
#include "Serialize/Serializable.h"
#include "Types.h"
#include <format>

namespace cstf {

using serialize::Serializable;

struct PlayerMoveEvent : Event<PlayerMoveEvent> {
    struct Data : public Serializable<PlayerMoveEvent> {
        /**
         * NOTE: Why is there a misalignment when casting a char[160] to
         *       PlayerMoveEvent when we pack the entire struct and not
         *       when we don't include the float3 in the packing?
         *
         *       In both cases sizeof(Data) == 20, so it should act the same.
         *       Unless there is something about how the spec/compiler that
         *       I do not understand.
         */
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