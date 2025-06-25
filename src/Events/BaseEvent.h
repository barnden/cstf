#pragma once

#include "Serialize/Serializable.h"
#include "Types.h"

namespace cstf {

using serialize::Serializable, serialize::Deserializer;

template <class Derived>
class Event : public IStringable<Event<Derived>>,
              public Serializable<Event<Derived>> {
public:
    Event() = default;

    static auto from(Deserializer<Derived> const& deserializer) -> Derived
    {
        auto instance = Derived {};
        deserializer.visit(instance);

        return instance;
    }
};

};