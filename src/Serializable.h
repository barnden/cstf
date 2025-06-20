#pragma once

#include "Types.h"

#ifndef NDEBUG
#    include <print>
#endif

namespace cstf::serialize {

template <typename T>
struct Serializer;

class BaseSerializer {
protected:
    ostream m_stream;

public:
    template <typename T>
    constexpr auto to() const -> Serializer<T> const&
    {
        return static_cast<Serializer<T> const&>(*this);
    }

    BaseSerializer(ostream const& stream)
        : m_stream(stream) { };

    void visit(auto const&) const
    {
#ifndef NDEBUG
        std::println("BaseSerializer::visit() should not be reached!");
#endif
#if _MSC_VER && !__INTEL_COMPILER
        __assume(false);
#else
        __builtin_unreachable();
#endif
    }

    auto stream() const -> ostream const& { return m_stream; }

    ~BaseSerializer() { };
};

template <typename T>
struct Deserializer;

class BaseDeserializer {
protected:
    istream m_stream;

public:
    template <typename T>
    constexpr auto to() const -> Deserializer<T> const&
    {
        return static_cast<Deserializer<T> const&>(*this);
    }

    BaseDeserializer(istream const& stream)
        : m_stream(stream) { };

    void visit(auto const&) const
    {
#ifndef NDEBUG
        std::println("BaseDeserializer::visit() should not be reached!");
#endif
#if _MSC_VER && !__INTEL_COMPILER
        __assume(false);
#else
        __builtin_unreachable();
#endif
    }

    auto stream() const -> istream const& { return m_stream; }

    ~BaseDeserializer() { };
};

template <typename T>
struct Serializer : BaseSerializer {
    template <typename U>
    constexpr auto to() const -> Serializer<U> const&
    {
        if constexpr (std::is_same_v<U, BaseSerializer>) {
            return static_cast<Serializer<U> const&>(to<BaseSerializer>());
        } else {
            return static_cast<BaseSerializer const&>(*this);
        }
    }

    void visit(T const& serializable) const
    {
        m_stream->write(reinterpret_cast<char const*>(&serializable), sizeof(T));
    }
};

template <typename T>
struct Deserializer : BaseDeserializer {
    template <typename U>
    constexpr auto to() const -> Deserializer<U> const&
    {
        if constexpr (std::is_same_v<U, BaseDeserializer>) {
            return static_cast<BaseDeserializer const&>(*this);
        } else {
            return static_cast<Deserializer<U> const&>(to<BaseDeserializer>());
        }
    }

    void visit(T& serializable) const
    {
        m_stream->read(reinterpret_cast<char*>(&serializable), sizeof(T));
    }
};

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