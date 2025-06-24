#pragma once

#include "Serializable.h"
#include "Types.h"

#include <bit>
#include <ranges>
#include <string>

namespace cstf {

using serialize::Serializable;

class Header : IStringable<Header>, public Serializable<Header> {
    static constexpr auto g_magic_bytes = std::array<u8, 3> { 0xC5, 0x7F, 0x8B };

public:
    enum class Flags {
        GSI = 1 << 0,
        MIRV = 1 << 1,
        KILLS = 1 << 2,
        DAMAGE = 1 << 3,
        POSITIONS = 1 << 4,
        UTILITY = 1 << 5
    };

    std::array<u8, 3> cstf_magic {};
    u8 flags {};
    u16 reserved {};
    u8 version {};
    u8 tick_rate {};
    std::string map_name {};
    u32 build_info {};

    Header() = default;

    Header(std::string const& map_name, u32 build_info, u8 flags, u8 tick_rate = 64)
        : cstf_magic(g_magic_bytes)
        , flags(flags)
        , version(1)
        , tick_rate(tick_rate)
        , map_name(map_name)
        , build_info(build_info) { };

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        using std::views::zip;

        for (auto const&& [byte, magic] : zip(cstf_magic, g_magic_bytes)) {
            if (byte != magic)
                return false;
        }

        return true;
    }

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format(
            "Header(magic: {:X}{:X}{:X}, flags: {:08B}, reserved: {:04X}, version: {}, tick_rate: {}, map: {}, build_info: {:x})",
            cstf_magic[0], cstf_magic[1], cstf_magic[2], flags, reserved, version,
            static_cast<int>(tick_rate), map_name, build_info);
    }
};

namespace serialize {
    template <>
    struct Serializer<Header> : BaseSerializer {
        void visit(Header const& header) const
        {
            m_stream->write(std::bit_cast<char const*>(&header.cstf_magic), sizeof(header.cstf_magic));

            m_stream << header.flags;

            m_stream->write(std::bit_cast<char const*>(&header.reserved), 2);

            m_stream << header.version << header.tick_rate;

            m_stream->write(header.map_name.c_str(), header.map_name.size() + 1);

            m_stream->write(std::bit_cast<char const*>(&header.build_info), 4);
        }
    };

    template <>
    struct Deserializer<Header> : public BaseDeserializer {
        void visit(Header& header) const
        {
            m_stream->read(std::bit_cast<char*>(&header.cstf_magic), sizeof(header.cstf_magic));

            m_stream >> header.flags;

            m_stream->read(std::bit_cast<char*>(&header.reserved), 2);

            m_stream >> header.version >> header.tick_rate;

            std::getline(*m_stream, header.map_name, '\0');

            m_stream->read(std::bit_cast<char*>(&header.build_info), 4);
        }
    };
}

};