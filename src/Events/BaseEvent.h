#pragma once

#include "Serializable.h"
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

namespace serialize {

    template <class Derived>
    struct Serializer<Event<Derived>> : BaseSerializer {
        void visit(Event<Derived> const& event) const
        {
            static_cast<Derived const&>(event).data.accept(to<BaseSerializer>());
        }
    };

    template <class Derived>
    struct Deserializer<Event<Derived>> : BaseDeserializer {
        void visit(Event<Derived>& event) const
        {
            static_cast<Derived&>(event).data.accept(to<BaseSerializer>());
        }
    };
}

};