#pragma once

#include "Types.h"

#include <fstream>
#include <ranges>
#include <string>

namespace CSTF {

class Header : IStringable<Header> {
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
    u8 flags = 0;
    u16 reserved = 0;
    u8 version = 0;
    u8 tick_rate = 0;
    std::string map_name {};
    u32 build_info = 0;

    Header() = default;

    Header(std::string const& map_name, u32 build_info, u8 flags, u8 tick_rate = 64)
        : cstf_magic(g_magic_bytes)
        , flags(flags)
        , version(1)
        , tick_rate(tick_rate)
        , map_name(map_name)
        , build_info(build_info) { };

    Header(istream stream)
    {
        stream->read(reinterpret_cast<char*>(&cstf_magic), sizeof(cstf_magic));

        stream >> flags;

        stream->read(reinterpret_cast<char*>(&reserved), 2);

        stream >> version >> tick_rate;

        std::getline(*stream, map_name, '\0');

        stream->read(reinterpret_cast<char*>(&build_info), 4);
    }

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

};