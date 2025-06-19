#pragma once

#include "Types.h"

namespace cstf::serialize {

template <class Derived, class Entry, class Data, size_t Alignment = 4>
class LookupTable;

class Serializer {
    ostream m_stream;

public:
    Serializer(ostream const& stream)
        : m_stream(stream) { };

    template <class T>
    void visit(T const& serializable) const
    {
        m_stream->write(reinterpret_cast<char const*>(&serializable), sizeof(T));
    }
};

class Deserializer {
    istream m_stream;

public:
    Deserializer(istream const& stream)
        : m_stream(stream) { };

    template <class T>
    void visit(T& serializable) const
    {
        m_stream->read(reinterpret_cast<char*>(&serializable), sizeof(T));
    }
};

template <typename Derived>
class Serializable {
public:
    void accept(auto const& serializer) const
    {
        serializer.visit(reinterpret_cast<Derived const&>(this));
    }

    void accept(auto& serializer)
    {
        serializer.visit(reinterpret_cast<Derived&>(this));
    }
};

}