#pragma once

#include <variant>

#include "PlayerDamageEvent.h"
#include "PlayerMoveEvent.h"

namespace CSTF {

// Adapted from https://stackoverflow.com/a/62139716
template <class... T>
struct type_array {
    using tuple_t = std::tuple<T...>;
    using variant_t = std::variant<T...>;

    template<size_t I>
    using get = std::tuple_element_t<I, tuple_t>;

    static constexpr size_t size = sizeof...(T);
};

using EventTypes = type_array<
    PlayerMoveEvent,
    PlayerDamageEvent>;

};