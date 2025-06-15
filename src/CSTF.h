#pragma once

#include <cstdint>
#include <format>
#include <fstream>
#include <string_view>

#if __cpp_lib_ranges_enumerate != 202302L
#    define enumerate(v) std::views::zip(std::views::iota(0uz), v)
#else
#    define enumerate(v) std::views::enumerate(v)
#endif

namespace CSTF {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

struct SteamID {
    u64 steam64;

    operator u64() const
    {
        return steam64;
    }

    [[nodiscard]] auto to_string() const -> std::string
    {
        static constexpr auto g_universes = "IUMGAPCgT a     ";
        u8 universe = (steam64 >> 56) & 0xF;
        u32 account = steam64;
        u8 data = account & 1;

        return std::format("[{}:{}:{}]", g_universes[universe], data, account);
    }
};

class Stream {
    std::ifstream& m_stream;
    std::ios_base::fmtflags m_flags;

public:
    Stream(std::ifstream& stream)
        : m_stream(stream)
        , m_flags(stream.flags())
    {
        m_stream >> std::noskipws;
    }

    template<size_t alignment>
    void consume_padding() const
    {
        char dummy;
        auto padding = (static_cast<size_t>(m_stream.tellg()) + alignment) % alignment;
        for (auto i = 0uz; i < padding; i++) {
            m_stream.read(&dummy, 1);
        }
    }

    void consume_padding(int alignment) const
    {
        char dummy;
        auto padding = (static_cast<size_t>(m_stream.tellg()) + alignment) % alignment;
        for (auto i = 0uz; i < padding; i++) {
            m_stream.read(&dummy, 1);
        }
    }

    [[nodiscard]] auto operator*() const -> std::ifstream& { return m_stream; }
    [[nodiscard]] auto operator->() const -> std::ifstream* { return &m_stream; }

    ~Stream() { m_stream.flags(m_flags); }
};

};

template <typename T>
concept Stringable = requires(T x) {
    { x.to_string() } -> std::same_as<std::string>;
};

namespace std {

template <Stringable T>
struct formatter<T> : formatter<string> {
    auto format(T const& instance, auto& context) const
    {
        return formatter<string>::format(instance.to_string(), context);
    }
};

}