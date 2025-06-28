#pragma once
#include "CSTF/Serialize/Serializer.h"

namespace cstf::serialize {

template <typename Derived>
class Serializable {
public:
    void accept(BaseSerializer const& visitor) const
    {
        visitor.to<Derived>().visit(reinterpret_cast<Derived const&>(*this));
    }

    void accept(BaseDeserializer const& visitor)
    {
        visitor.to<Derived>().visit(reinterpret_cast<Derived&>(*this));
    }
};

}