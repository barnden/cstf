#pragma once

#include "CSTF/Utility/Types.h"

#include <bit>
#include <bitset>
#include <type_traits>
#ifndef NDEBUG
#    include <print>
#endif

namespace cstf::serialize {

template <class Stream, template <class T> class Derived>
class BaseSerializationVisitor {
protected:
    Stream m_stream;

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

    template <typename T>
    void operator()(T& host) const
    {
        host.accept(to<T>());
    }

    template <typename T>
    void operator()(T const& host) const
    {
        host.accept(to<T>());
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
    enum Flags {
        ReadRoundLUT,
        ReadEventLUT,
        ReadEventData,
        Flags_MAX
    };
    using flag_t = std::bitset<static_cast<size_t>(Flags::Flags_MAX)>;

protected:
    flag_t m_flags;

public:
    BaseDeserializer(istream const& stream)
        : BaseDeserializer(stream, flag_t {}.set()) { };

    BaseDeserializer(istream const& stream, flag_t flags)
        : BaseSerializationVisitor<istream, Deserializer>(stream)
        , m_flags(flags) { };

    auto options() noexcept -> flag_t&
    {
        return m_flags;
    }

    template <typename T>
    void operator()(T& host) const
    {
        if (!m_flags[Flags::ReadRoundLUT]) {
            if (m_flags[Flags::ReadEventLUT]) {
                std::println("[Deserializer] ReadEventLUT is ignored because ReadRoundLUT is unset.");
            }

            if (m_flags[Flags::ReadEventData]) {
                std::println("[Deserializer] ReadEventData is ignored because ReadRoundLUT is unset.");
            }
        }

        if (m_flags[Flags::ReadEventData] && !m_flags[Flags::ReadEventLUT]) {
            std::println("[Deserializer] ReadEventData is ignored because ReadEventLUT is unset.");
        }

        BaseSerializationVisitor<istream, Deserializer>::operator()(host);
    }
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
    constexpr auto to() const -> auto const&
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

}