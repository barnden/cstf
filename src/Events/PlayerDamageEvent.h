#pragma once

#include "Types.h"
#include <format>

namespace CSTF {

#pragma pack(1)
struct PlayerDamageEvent {
    u8 source {};
    u8 armor {};
    u8 health {};
    u8 victim : 4 {};
    u8 attacker : 4 {};

    auto to_string() const -> std::string
    {
        return std::format("CSTFPlayerDamageEvent(attacker: {}, victim: {}, health: {}, armor: {}, source: {})",
                           attacker,
                           victim,
                           health,
                           armor,
                           source);
    }
};
#pragma pack()

};