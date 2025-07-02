#pragma once

#include "CSTF/Serialize/Serializable.h"
#include "CSTF/Utility/Types.h"

#include <initializer_list>
#include <ranges>
#include <string>

namespace cstf {

using serialize::Serializable;

class Header : IStringable<Header>, public Serializable<Header> {
    static constexpr auto g_magic_bytes = std::array<u8, 3> { 0xC5, 0x7F, 0x8B };

    friend serialize::Serializer<Header>;
    friend serialize::Deserializer<Header>;

public:
    enum Flags : u8 {
        GSI,
        MIRV,
        KILLS,
        DAMAGE,
        POSITIONS,
        UTILITY,
        RESERVED_0,
        RESERVED_1,
    };

    using flag_t = std::bitset<8>;

private:
    std::array<u8, 3> m_cstf_magic {};
    flag_t m_flags {};
    u16 m_reserved {};
    u8 m_version {};
    u8 m_tick_rate {};
    std::string m_map_name {};
    u32 m_build_info {};

public:
    Header() = default;

    Header(std::string const& map_name, u32 build_info, flag_t flags, u8 tick_rate = 64)
        : m_cstf_magic(g_magic_bytes)
        , m_flags(flags)
        , m_version(1)
        , m_tick_rate(tick_rate)
        , m_map_name(map_name)
        , m_build_info(build_info) { };

    Header(std::string const& map_name, u32 build_info, std::initializer_list<Flags> flags, u8 tick_rate = 64)
        : m_cstf_magic(g_magic_bytes)
        , m_flags()
        , m_version(1)
        , m_tick_rate(tick_rate)
        , m_map_name(map_name)
        , m_build_info(build_info)
    {
        for (auto&& flag : flags) {
            m_flags.set(flag);
        }
    }

    [[nodiscard]] constexpr auto flags() const noexcept { return m_flags; }

    [[nodiscard]] constexpr auto version() const noexcept { return m_version; }

    [[nodiscard]] constexpr auto tick_rate() const noexcept { return m_tick_rate; }

    [[nodiscard]] constexpr auto map_name() const noexcept { return m_map_name; }

    [[nodiscard]] constexpr auto build_info() const noexcept { return m_build_info; }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        using std::views::zip;

        for (auto const&& [byte, magic] : zip(m_cstf_magic, g_magic_bytes)) {
            if (byte != magic)
                return false;
        }

        return true;
    }

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        static constexpr auto g_flags = std::array { 'G', 'M', 'K', 'H', 'P', 'U', 'x', 'x' };

        std::string flag_string {};
        u8 flags = m_flags.to_ulong();

        for (auto i = 0; i < static_cast<int>(Flags::RESERVED_0); i++, flags >>= 1) {
            if ((flags & 1) == 0)
                continue;

            flag_string += g_flags[i];
        }

        return std::format(
            "Header(magic: {:X}{:X}{:X}, flags: {}, reserved: {:04X}, version: {}, tick_rate: {}, map: {}, build_info: {:x})",
            m_cstf_magic[0], m_cstf_magic[1], m_cstf_magic[2],
            flag_string,
            m_reserved,
            m_version,
            static_cast<int>(m_tick_rate),
            m_map_name,
            m_build_info);
    }
};

};