#pragma once

#include "Types.h"

#include <string>
#include <vector>

namespace CSTF {

struct GameData : IStringable<GameData>, ISerializable<GameData> {
    std::vector<SteamID> players { 10 };
    std::vector<std::string> usernames { 10 };
    std::array<std::string, 2> team_tags;
    std::array<std::string, 2> team_names;

    GameData() = default;

    void deserialize(istream const& stream)
    {
        stream.consume_padding(4);

        u32 player_count {};
        player_count &= 0xFF;

        stream->read(reinterpret_cast<char*>(&player_count), 4);

        players.resize(player_count);
        usernames.resize(player_count);

        for (auto&& player : players) {
            stream->read(reinterpret_cast<char*>(&player), 8);
        }

        for (auto&& username : usernames) {
            std::getline(*stream, username, '\0');
        }

        for (auto&& tag : team_tags) {
            std::getline(*stream, tag, '\0');
        }

        for (auto&& name : team_names) {
            std::getline(*stream, name, '\0');
        }
    }

    void serialize(ostream const& stream) const
    {
        stream.pad(4);

        u32 player_count = players.size();
        stream->write(reinterpret_cast<char const*>(&player_count), 4);

        for (auto&& player : players) {
            stream->write(reinterpret_cast<char const*>(&player), 8);
        }

        for (auto&& username : usernames) {
            stream->write(username.c_str(), username.size() + 1);
        }

        for (auto&& tag : team_tags) {
            stream->write(tag.c_str(), tag.size() + 1);
        }

        for (auto&& name : team_names) {
            stream->write(name.c_str(), name.size() + 1);
        }
    }

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