#pragma once

#include "CSTF/Serialize/Serializable.h"
#include "CSTF/Utility/Types.h"

#include <string>
#include <vector>

namespace cstf {

using serialize::Serializable;

class GameData : IStringable<GameData>, public Serializable<GameData> {
    std::vector<SteamID> m_players {};
    std::vector<std::string> m_usernames {};
    std::array<std::string, 2> m_team_tags;
    std::array<std::string, 2> m_team_names;

    friend serialize::Serializer<GameData>;
    friend serialize::Deserializer<GameData>;

public:
    GameData() = default;

    void add_player(SteamID const& steam64, std::string const& username)
    {
        m_players.push_back(steam64);
        m_usernames.push_back(username);
    }

    void set_team(size_t team, std::string const& name, std::string const& tag)
    {
        m_team_tags[team] = tag;
        m_team_names[team] = name;
    }

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format(
            "GameData(steamids: {}, tags: {}, teams: {}, usernames: {})",
            m_players,
            m_team_tags,
            m_team_names,
            m_usernames);
    }
};

};