#pragma once

#include <type_traits>
#include <variant>

#include "CSTF/Format/Events/PlayerDamageEvent.h"
#include "CSTF/Format/Events/PlayerMoveEvent.h"

namespace cstf {

// Adapted from https://stackoverflow.com/a/62139716
template <class... Ts>
struct type_array {
    using tuple_t = std::tuple<Ts...>;
    using variant_t = std::variant<Ts...>;

    template <size_t I>
    using get = std::tuple_element_t<I, tuple_t>;

    template <typename T, typename U, typename... Types>
    static constexpr auto index(size_t i = 0uz) -> size_t
    {
        if constexpr (std::is_same_v<T, U>) {
            return i;
        } else {
            return index<T, Types...>(i + 1);
        }
    }

    template <typename T>
    static constexpr auto index() -> size_t
    {
        return index<T, Ts...>();
    }

    static constexpr size_t size = sizeof...(Ts);

    static_assert(
        std::conjunction_v<std::is_trivially_copyable<typename Ts::Data>...>,
        "All event data must be trivially copyable.");
};

using EventTypes = type_array<
    PlayerMoveEvent,
    PlayerDamageEvent>;

};
