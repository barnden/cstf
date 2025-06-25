#pragma once

#include "Serialize/Serializable.h"
#include "Types.h"

#include <bit>
#include <string>
#include <vector>

namespace cstf {

using serialize::Serializable;

class GameData : IStringable<GameData>, public Serializable<GameData> {
    std::vector<SteamID> players { 10 };
    std::vector<std::string> usernames { 10 };
    std::array<std::string, 2> team_tags;
    std::array<std::string, 2> team_names;

    friend serialize::Serializer<GameData>;
    friend serialize::Deserializer<GameData>;

public:
    GameData() = default;

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format(
            "GameData(steamids: {}, tags: {}, teams: {}, usernames: {})",
            players,
            team_tags,
            team_names,
            usernames);
    }
};

};