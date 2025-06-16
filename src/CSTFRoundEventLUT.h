#pragma once

#include "CSTF.h"
#include "CSTFLookupTable.h"
#include <variant>

namespace CSTF {

#pragma pack(1)
struct CSTFRoundEventLUTEntry {
    u32 offset : 24 {};
    u16 next : 15 {};
    u8 frames : 3 {};
    u8 type : 6 {};

    [[nodiscard]] auto to_string() const -> std::string
    {
        return std::format("CSTFRoundEventLUTEntry(type: {:06b}, frames: {}, next: {:04x} offset: {:06x})",
                           type, frames, next, offset);
    }
};
#pragma pack()

static_assert(sizeof(CSTFRoundEventLUTEntry) == 6);

#pragma pack(1)
struct CSTFPlayerMoveEvent {
    u16 yaw : 15 {};
    u16 pitch : 13 {};
    u8 player : 4 {};
    float position[3] {};

    auto to_string() const -> std::string
    {
        float pitch_angle = static_cast<float>(pitch) / ((1ul << 13) - 1) * 180.;
        float yaw_angle = static_cast<float>(yaw) / ((1ul << 15) - 1) * 360.;
        return std::format("CSTFPlayerMoveEvent(player: {}, pitch: {:0.3f}, yaw: {:0.3f}, position: {})",
                           player,
                           pitch_angle,
                           yaw_angle,
                           position);
    }
};
#pragma pack()

#pragma pack(1)
struct CSTFPlayerDamageEvent {
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

using CSTFEventTypes = std::tuple<CSTFPlayerMoveEvent, CSTFPlayerDamageEvent>;

// FIXME: Move this elsewhere
// Copied from https://www.reddit.com/r/cpp/comments/l37uui/comment/gkdag33/
template <typename T, T... S, typename F>
constexpr void for_sequence(std::integer_sequence<T, S...>, F f)
{
    (static_cast<void>(f(std::integral_constant<T, S> {})), ...);
}

template <auto n, typename F>
constexpr void for_sequence(F f)
{
    for_sequence(std::make_integer_sequence<decltype(n), n> {}, f);
}

struct CSTFRoundEventLUT : public CSTFLookupTable<CSTFRoundEventLUTEntry> {
    // FIXME (1): vector of variants makes me sad ;(
    // FIXME (2): if we keep using variants probably look at extracting T... from
    //            the CSTFEventTypes tuple<T...> to make this less redundant
    std::vector<std::variant<CSTFPlayerMoveEvent, CSTFPlayerDamageEvent>> events {};

    CSTFRoundEventLUT() = default;
    CSTFRoundEventLUT(Stream stream)
        : CSTFLookupTable<CSTFRoundEventLUTEntry>(stream)
    {
        events.reserve(entries.size());

        stream.consume_padding<4>();
        size_t start = stream->tellg();

        for (auto&& entry : entries) {
            u32 offset = entry.offset;
            auto position = start + 4 * offset;
            stream->seekg(position);

            for_sequence<2>([&](auto i) {
                using Event = std::tuple_element_t<i, CSTFEventTypes>;

                if (entry.type != i)
                    return;

                auto event = Event {};
                stream->read(reinterpret_cast<char*>(&event), sizeof(Event));

                events.push_back(event);
            });
        }
    }
};
};