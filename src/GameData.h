#pragma once

#include "Types.h"

#include <fstream>
#include <string>

namespace CSTF {

struct GameData {
    std::array<u64, 10> players;
    std::array<std::string, 2> team_tags;
    std::array<std::string, 2> team_names;
    std::array<std::string, 10> usernames;

    GameData() = default;
    GameData(Stream stream)
    {
        stream.consume_padding<4>();

        for (auto&& player : players) {
            stream->read(reinterpret_cast<char*>(&player), 8);
        }

        for (auto&& tag : team_tags) {
            std::getline(*stream, tag, '\0');
        }

        for (auto&& name : team_names) {
            std::getline(*stream, name, '\0');
        }

        for (auto&& username : usernames) {
            std::getline(*stream, username, '\0');
        }
    }

    [[nodiscard]] auto to_string() const -> std::string
    {
        std::string steamids {};

        steamids += "[";
        for (auto const&& [i, player] : enumerate(players)) {
            steamids += SteamID(player).to_string();

            if (i != static_cast<int>(players.size()) - 1)
                steamids += ", ";
        }
        steamids += "]";

        return std::format(
            "CSTFGameData(steamids: {}, tags: {}, teams: {}, usernames: {})",
            steamids,
            team_tags,
            team_names,
            usernames);
    }
};

};