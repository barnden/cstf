#pragma once

#include "Types.h"

namespace CSTF {

template <class Derived>
class Event : public IStringable<Event<Derived>>,
              public ISerializable<Event<Derived>> {
public:
    Event() = default;

    static auto from(istream const& stream) -> Derived
    {
        auto instance = Derived {};
        instance.data.deserialize(stream);

        return instance;
    }

    void deserialize(istream const& stream)
    {
        Derived& self = *static_cast<Derived*>(this);
        self.data.deserialize(stream);
    }

    void serialize(ostream const& stream) const
    {
        Derived const& self = *static_cast<Derived const*>(this);
        self.data.serialize(stream);
    }
};

};