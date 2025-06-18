#pragma once

#include "Types.h"

namespace CSTF {

template <class Derived>
class Event : public IStringable<Event<Derived>> {
public:
    Event() = default;

    static auto from(istream const& stream) -> Derived
    {
        auto instance = Derived {};
        instance.data.deserialize(stream);

        return instance;
    }
};

};