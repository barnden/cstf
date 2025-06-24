#pragma once

#include <variant>
#include <type_traits>

#include "PlayerDamageEvent.h"
#include "PlayerMoveEvent.h"

namespace cstf {

// Adapted from https://stackoverflow.com/a/62139716
template <class... T>
struct type_array {
    using tuple_t = std::tuple<T...>;
    using variant_t = std::variant<T...>;

    template <size_t I>
    using get = std::tuple_element_t<I, tuple_t>;

    static constexpr size_t size = sizeof...(T);

    static_assert(std::conjunction_v<std::is_trivially_copyable<typename T::Data>...>, "All event data must be trivially copyable.");
};

using EventTypes = type_array<
    PlayerMoveEvent,
    PlayerDamageEvent>;

};
