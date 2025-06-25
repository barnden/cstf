#pragma once

#include "Serialize/Serializable.h"
#include "Types.h"
#include <Serialize/Serializer.h>

namespace cstf {

using serialize::Serializable, serialize::Deserializer;

template <class Derived>
class Event : public IStringable<Event<Derived>>,
              public Serializable<Event<Derived>> {
public:
    Event() = default;
};

};