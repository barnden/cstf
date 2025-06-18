#pragma once

#include <print>
#include <variant>
#include <vector>

#include "GameData.h"
#include "Header.h"
#include "Tables/EventLUT.h"
#include "Tables/RoundLUT.h"
#include "Types.h"

namespace CSTF {

class CSTF : public IStringable<CSTF>, public ISerializable<CSTF> {
    Header m_header {};
    GameData m_game_data {};
    RoundLUT m_rounds {};

    std::vector<EventLUT> m_events {};

public:
    CSTF() = default;

    void deserialize(istream const& stream)
    {
        m_header.deserialize(stream);
        m_game_data.deserialize(stream);
        m_rounds.deserialize(stream);

        stream.consume_padding(RoundLUT::alignment);
        size_t event_base = stream->tellg();

        for (auto&& entry : m_rounds.m_entries) {
            if (entry.type != RoundLUTEntry::Type::ROUND)
                continue;

            auto position = event_base + 4 * entry.offset;
            stream->seekg(position);

            auto lut = EventLUT {};
            lut.deserialize(stream);

            m_events.push_back(lut);
        }
    }

    void serialize(ostream const& stream) const {
        m_header.serialize(stream);
        m_game_data.serialize(stream);
    }

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        using std::views::zip;

        std::string result;
        auto cur_round = 0;

        std::println("{}", m_header);
        std::println("{}", m_game_data);

        for (auto&& round : m_rounds.m_entries) {
            std::println("{}", round);

            if (round.type != RoundLUTEntry::Type::ROUND)
                continue;

            auto const& events = m_events[cur_round++];
            for (auto const&& [event, data] : zip(events.m_entries, events.events)) {
                std::println("\t{}", event);
                std::visit(
                    [](auto const& e) { std::println("\t\t{}", e); },
                    data);
            }
        }

        return result;
    }
};

};