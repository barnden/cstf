#pragma once

#include "BaseEvent.h"
#include "Types.h"
#include <format>

namespace cstf {

struct PlayerDamageEvent : Event<PlayerDamageEvent> {
#pragma pack(1)
    struct Data : public ISerializable<Data> {
        u8 padding {};
        u8 source {};
        u8 armor {};
        u8 health {};
        u8 victim {};
        u8 attacker {};
    } data {};
#pragma pack()

    PlayerDamageEvent() = default;

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format("PlayerDamageEvent(attacker: {}, victim: {}, health: {}, armor: {}, source: {})",
                           data.attacker,
                           data.victim,
                           data.health,
                           data.armor,
                           data.source);
    }
};

};