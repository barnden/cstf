#pragma once

#include "CSTF/Format/GameData.h"
#include "CSTF/Serialize/Serializer.h"

#include <bit>

namespace cstf::serialize {
template <>
struct Serializer<GameData> : BaseSerializer {
    void visit(GameData const& game) const
    {
        m_stream.pad(4);

        u32 player_count = game.players.size();
        m_stream->write(std::bit_cast<char const*>(&player_count), 4);

        for (auto&& player : game.players) {
            m_stream->write(std::bit_cast<char const*>(&player), 8);
        }

        for (auto&& username : game.usernames) {
            m_stream->write(username.c_str(), username.size() + 1);
        }

        for (auto&& tag : game.team_tags) {
            m_stream->write(tag.c_str(), tag.size() + 1);
        }

        for (auto&& name : game.team_names) {
            m_stream->write(name.c_str(), name.size() + 1);
        }
    }
};

template <>
struct Deserializer<GameData> : BaseDeserializer {
    void visit(GameData& game) const
    {
        m_stream.consume_padding(4);

        u32 player_count {};
        player_count &= 0xFF;

        m_stream->read(std::bit_cast<char*>(&player_count), 4);

        game.players.resize(player_count);
        game.usernames.resize(player_count);

        for (auto&& player : game.players) {
            m_stream->read(std::bit_cast<char*>(&player), 8);
        }

        for (auto&& username : game.usernames) {
            std::getline(*m_stream, username, '\0');
        }

        for (auto&& tag : game.team_tags) {
            std::getline(*m_stream, tag, '\0');
        }

        for (auto&& name : game.team_names) {
            std::getline(*m_stream, name, '\0');
        }
    }
};
}