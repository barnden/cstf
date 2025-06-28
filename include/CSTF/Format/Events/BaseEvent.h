#pragma once

#include "CSTF/Serialize/Serializable.h"
#include "CSTF/Utility/Types.h"

namespace cstf {

using cstf::serialize::Serializable;

template <class Derived>
class Event : public IStringable<Event<Derived>>,
              public Serializable<Event<Derived>> {
public:
    Event() = default;
};

};