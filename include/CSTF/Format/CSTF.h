#pragma once

#include <initializer_list>
#include <vector>

#include "CSTF/Format/GameData.h"
#include "CSTF/Format/Header.h"
#include "CSTF/Format/Tables/EventLUT.h"
#include "CSTF/Format/Tables/RoundLUT.h"
#include "CSTF/Serialize/Serializable.h"
#include "CSTF/Utility/Types.h"

namespace cstf {

using namespace cstf::serialize;

class CSTF : public IStringable<CSTF>,
             public Serializable<CSTF> {
    Header m_header {};
    GameData m_game_data {};
    RoundLUT m_rounds {};

    friend Serializer<CSTF>;
    friend Deserializer<CSTF>;

public:
    CSTF() = default;

    CSTF(std::string const& map_name, u32 build_info, Header::flag_t flags, u8 tick_rate = 64)
        : m_header(map_name, build_info, flags, tick_rate) { };

    CSTF(std::string const& map_name, u32 build_info, std::initializer_list<Header::Flags> flags, u8 tick_rate = 64)
        : m_header(map_name, build_info, flags, tick_rate) { };

    void add_player(SteamID const& steam64, std::string const& username)
    {
        m_game_data.add_player(steam64, username);
    }

    void add_round(RoundLUTEntry::Type type, u32 frame)
    {
        m_rounds.add(type, frame);
    }

    void add_round(RoundLUTEntry::Type type, u32 frame, EventLUT&& events)
    {
        m_rounds.add(type, frame, std::move(events));
    }

    void set_team(size_t team, std::string const& name, std::string const& tag)
    {
        m_game_data.set_team(team, name, tag);
    }

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        std::string result {};
        auto cur_round = 0;

        result += std::format("{}\n", m_header);
        result += std::format("{}\n", m_game_data);

        // FIXME: Move this logic to the appropriate to_string() methods in the respective classes
        for (auto&& round : m_rounds.entries()) {
            result += std::format("{}\n", round);

            if (round.type != RoundLUTEntry::Type::ROUND)
                continue;

            if (m_rounds.data().size() == 0)
                continue;

            auto const& events = m_rounds.data().at(cur_round++);

            result += std::format("{}\n", events);
        }

        return result;
    }
};
}