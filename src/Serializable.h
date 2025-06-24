#pragma once

#include "Types.h"

#include <bit>
#include <type_traits>
#ifndef NDEBUG
#    include <print>
#endif

namespace cstf::serialize {

template <class Stream, template <class T> class Derived>
class BaseSerializationVisitor {
protected:
    Stream m_stream;

public:
    BaseSerializationVisitor(Stream const& stream)
        : m_stream(stream) { };

    template <typename T>
    constexpr auto to() const -> Derived<T> const&
    {
        return static_cast<Derived<T> const&>(*this);
    }

    template <typename T>
    constexpr auto to() -> Derived<T>&
    {
        return static_cast<Derived<T>&>(*this);
    }

    [[nodiscard]] constexpr auto stream() const noexcept
    {
        return m_stream;
    }

    void visit(auto const&) const
    {
#ifndef NDEBUG
        std::println("BaseSerializationVisitor::visit(const&) should not be reached!");
#endif

        ASSERT_NOT_REACHED;
    }

    void visit(auto&) const
    {
#ifndef NDEBUG
        std::println("BaseSerializationVisitor::visit(&) should not be reached!");
#endif

        ASSERT_NOT_REACHED;
    }
};

template <typename T>
struct Serializer;

class BaseSerializer : public BaseSerializationVisitor<ostream, Serializer> {
public:
    BaseSerializer(ostream const& stream)
        : BaseSerializationVisitor<ostream, Serializer>(stream) { };
};

template <typename T>
struct Deserializer;

class BaseDeserializer : public BaseSerializationVisitor<istream, Deserializer> {
public:
    BaseDeserializer(istream const& stream)
        : BaseSerializationVisitor<istream, Deserializer>(stream) { };
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
        if constexpr (std::is_trivially_copyable_v<T>) {
            m_stream->write(std::bit_cast<char*>(&serializable), sizeof(T));
        } else {
            std::println("Serializer<{}> does not implement default visit() member for non-trivally copyable types.", demangle(typeid(T)));
            ASSERT_NOT_REACHED;
        }
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
        if constexpr (std::is_trivially_copyable_v<T>) {
            m_stream->read(std::bit_cast<char*>(&serializable), sizeof(T));
        } else {
            std::println("Deserializer<{}> does not implement default visit() member for non-trivally copyable types.", demangle(typeid(T)));
            ASSERT_NOT_REACHED;
        }
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