#pragma once

#include "Events/BaseEvent.h"
#include "Serialize/Serializer.h"

namespace cstf::serialize {

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