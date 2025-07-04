#pragma once

#include <type_traits>
#include <variant>

#include "CSTF/Format/Events/PlayerDamageEvent.h"
#include "CSTF/Format/Events/PlayerMoveEvent.h"

namespace cstf {

// Adapted from https://stackoverflow.com/a/62139716
template <class... Ts>
// clang-format off
    requires (std::is_nothrow_convertible_v<Ts, Event<Ts>> && ...) &&
             (std::is_trivially_copyable_v<typename Ts::Data> && ...)
// clang-format on
struct type_array {
    using tuple_t = std::tuple<Ts...>;
    using variant_t = std::variant<Ts...>;

    template <size_t I>
    using get = std::tuple_element_t<I, tuple_t>;

    template <typename T, typename U, typename... Types>
    static constexpr auto index_impl(size_t i = 0uz) -> size_t
    {
        if constexpr (std::is_same_v<T, U>) {
            return i;
        } else {
            return index_impl<T, Types...>(i + 1);
        }
    }

    template <class T>
        requires(std::is_nothrow_convertible_v<T, Event<T>>)
    static constexpr auto index_of = index_impl<T, Ts...>();

    static constexpr size_t size = sizeof...(Ts);
};

using EventTypes = type_array<
    PlayerMoveEvent,
    PlayerDamageEvent>;

};
