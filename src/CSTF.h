#pragma once

#include <variant>
#include <vector>

#include "GameData.h"
#include "Header.h"
#include "Tables/EventLUT.h"
#include "Tables/RoundLUT.h"
#include "Types.h"

namespace CSTF {

class CSTF {
    Header m_header;
    GameData m_game_data;
    RoundLUT m_rounds;

    std::vector<EventLUT> m_events {};

public:
    CSTF(Stream stream)
    {
        m_header = Header(stream);
        m_game_data = GameData(stream);
        m_rounds = RoundLUT(stream);

        stream.consume_padding<RoundLUT::alignment>();
        size_t event_base = stream->tellg();

        for (auto&& entry : m_rounds.m_entries) {
            if (entry.type != RoundLUTEntry::Type::ROUND)
                continue;

            auto position = event_base + 4 * entry.offset;
            stream->seekg(position);

            m_events.emplace_back(stream);
        }
    }

    [[nodiscard]] auto to_string() const -> std::string
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