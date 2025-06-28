#pragma once

#include <fstream>

namespace cstf {

class istream {
    std::istream& m_stream;
    std::ios_base::fmtflags m_flags;

public:
    istream(std::istream& stream)
        : m_stream(stream)
        , m_flags(stream.flags())
    {
        m_stream >> std::noskipws;
    }

    auto consume_padding(int alignment) const -> size_t
    {
        auto remainder = static_cast<size_t>(m_stream.tellg()) % alignment;

        if (remainder == 0)
            return 0;

        auto padding = alignment - remainder;
        char dummy;

        for (auto i = 0uz; i < padding; i++)
            m_stream.read(&dummy, 1);

        return padding;
    }

    template <typename T>
    auto operator>>(T& rhs) const -> istream const&
    {
        m_stream >> rhs;
        return *this;
    }

    [[nodiscard]] auto operator*() const -> std::istream& { return m_stream; }
    [[nodiscard]] auto operator->() const -> std::istream* { return &m_stream; }

    ~istream() { m_stream.flags(m_flags); }
};

class ostream {
    std::ostream& m_stream;
    std::ios_base::fmtflags m_flags;

public:
    ostream(std::ostream& stream)
        : m_stream(stream)
        , m_flags(stream.flags())
    {
    }

    auto pad(int alignment) const -> size_t
    {
        auto remainder = static_cast<size_t>(m_stream.tellp()) % alignment;

        if (remainder == 0)
            return 0;

        auto padding = alignment - remainder;
        char dummy = '\0';

        for (auto i = 0uz; i < padding; i++)
            m_stream.write(&dummy, 1);

        return padding;
    }

    template <typename T>
    auto operator<<(T& rhs) const -> ostream const&
    {
        m_stream << rhs;
        return *this;
    }

    [[nodiscard]] auto operator*() const -> std::ostream& { return m_stream; }
    [[nodiscard]] auto operator->() const -> std::ostream* { return &m_stream; }

    ~ostream() { m_stream.flags(m_flags); }
};

}