#pragma once

#include "Types.h"

#include <cxxabi.h>
#include <vector>

namespace CSTF {

template <class Entry, size_t Alignment = 4>
class LookupTable : IStringable<LookupTable<Entry, Alignment>> {
public:
    static constexpr size_t alignment = Alignment;
    std::vector<Entry> m_entries {};

    LookupTable() = default;
    LookupTable(istream stream)
    {
        stream.consume_padding(Alignment);

        u32 num_bytes = 0;
        stream->read(reinterpret_cast<char*>(&num_bytes), 4);

        auto size = num_bytes / sizeof(Entry);
        m_entries = std::vector<Entry>(size);

        stream->read(reinterpret_cast<char*>(m_entries.data()), num_bytes);
    }

    virtual ~LookupTable() = default;

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        int status {};

        char* type = abi::__cxa_demangle(typeid(Entry).name(), NULL, NULL, &status);
        std::string result = std::format("LookupTable<{}>(size: {})", type, m_entries.size());
        free(type);

        return result;
    }

    auto entries() const -> std::vector<Entry> const&
    {
        return m_entries;
    }
};

};