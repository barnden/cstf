#pragma once

#include "Types.h"

#include <cxxabi.h>
#include <vector>

namespace CSTF {

template <class Entry, class Data, size_t Alignment = 4>
class LookupTable : IStringable<LookupTable<Entry, Data, Alignment>>,
                    public ISerializable<LookupTable<Entry, Data, Alignment>> {
protected:
    std::vector<Entry> m_entries {};
    std::vector<Data> m_data {};

public:
    static constexpr size_t alignment = Alignment;

    LookupTable() = default;
    virtual ~LookupTable() = default;

    void deserialize(istream const& stream)
    {
        stream.consume_padding(Alignment);

        u32 num_bytes = 0;
        stream->read(reinterpret_cast<char*>(&num_bytes), 4);

        auto size = num_bytes / sizeof(Entry);
        m_entries = std::vector<Entry>(size);

        stream->read(reinterpret_cast<char*>(m_entries.data()), num_bytes);
    }

    void serialize(ostream const& stream) const
    {
        stream.pad(Alignment);

        u32 num_bytes = m_entries.size() * sizeof(Entry);
        stream->write(reinterpret_cast<char const*>(&num_bytes), 4);

        for (auto&& entry : m_entries) {
            stream->write(reinterpret_cast<char const*>(&entry), sizeof(Entry));
        }
    }

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

    auto data() const -> std::vector<Data> const&
    {
        return m_data;
    }

    virtual void add(Entry& entry)
    {
        m_entries.push_back(entry);
    }
};

};