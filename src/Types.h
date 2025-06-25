#pragma once

#include <concepts>
#include <cstdint>
#include <cxxabi.h>
#include <format>
#include <ranges>
#include <type_traits>

#include "Stream.h"

#if _MSC_VER && !__INTEL_COMPILER
#    define ASSERT_NOT_REACHED __assume(false);
#else
#    define ASSERT_NOT_REACHED __builtin_unreachable();
#endif

[[nodiscard]] constexpr auto demangle(std::type_info const& info) -> std::string
{
    int status {};
    char* name = abi::__cxa_demangle(info.name(), NULL, NULL, &status);
    std::string result { name };
    free(name);

    return result;
}

// Copied from https://www.reddit.com/r/cpp/comments/l37uui/comment/gkdag33/
template <typename T, T... S, typename F>
constexpr void for_sequence(std::integer_sequence<T, S...>, F f)
{
    (static_cast<void>(f(std::integral_constant<T, S> {})), ...);
}

template <auto n, typename F>
constexpr void for_sequence(F f)
{
    for_sequence(std::make_integer_sequence<decltype(n), n> {}, f);
}

template <class Derived>
class IStringable {
    // Use CRTP interface instead of virtual so as to not induce vtable overhead
public:
    IStringable() = default;

    constexpr auto to_string() const noexcept -> std::string
    {

        std::string result = std::format("{} does not implement to_string()", demangle(typeid(Derived)));

        return result;
    }
};

// AFAIK there is not an easy way to do something like is_base_of_v<IStringable, T>
// with CRTP that also captures grandchildren, as in the case with BaseEvent
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

namespace cstf {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

#if __cpp_lib_ranges_enumerate != 202302L
constexpr auto enumerate(auto&& v)
{
    return std::views::zip(std::views::iota(0), v);
}
#else
constexpr auto enumerate(auto&& v)
{
    return std::views::enumerate(v);
}
#endif

struct SteamID : public IStringable<SteamID> {
    u64 steam64;

    operator u64() const
    {
        return steam64;
    }

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        static constexpr auto g_universes = "IUMGAPCgT a     ";
        u8 universe = (steam64 >> 56) & 0xF;
        u32 account = steam64;
        u8 data = account & 1;

        return std::format("[{}:{}:{}]", g_universes[universe], data, account);
    }
};

struct float3 {
    float x;
    float y;
    float z;
};

};
