#pragma once

#include "CSTF.h"

#include <fstream>
#include <ranges>
#include <string>

namespace CSTF {

struct CSTFGameData {
    std::array<u64, 10> players;
    std::array<std::string, 2> team_tags;
    std::array<std::string, 2> team_names;
    std::array<std::string, 10> usernames;

    CSTFGameData() = default;
    CSTFGameData(Stream stream)
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
        for (auto&& [i, player] : enumerate(players)) {
            steamids += SteamID(player).to_string();

            if (i != players.size() - 1uz)
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