#pragma once

#include "Types.h"

#include <cxxabi.h>
#include <vector>

namespace cstf {

template <class Derived, class Entry, class Data, size_t Alignment = 4>
class LookupTable : IStringable<LookupTable<Derived, Entry, Data, Alignment>>,
                    public ISerializable<LookupTable<Derived, Entry, Data, Alignment>> {
protected:
    std::vector<Entry> m_entries {};
    std::vector<Data> m_data {};
    u32 m_offset_size = 4;

    void deserialize_data(
        istream const& stream,
        Entry const& entry,
        size_t base)
    {
        size_t position = base + m_offset_size * entry.offset;
        stream->seekg(position);

        m_data.emplace_back();
        m_data.back().deserialize(stream);
    }

    void serialize_data(ostream const& stream, Data const& data) const
    {
        data.serialize(stream);
    }

public:
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
        stream.consume_padding(Alignment);

        size_t base = stream->tellg();
        for (auto&& entry : m_entries) {
            reinterpret_cast<Derived*>(this)->deserialize_data(stream, entry, base);
        }
    }

    void serialize(ostream const& stream) const
    {
        stream.pad(Alignment);

        u32 num_bytes = m_entries.size() * sizeof(Entry);
        stream->write(reinterpret_cast<char const*>(&num_bytes), 4);

        for (auto&& entry : m_entries) {
            stream->write(reinterpret_cast<char const*>(&entry), sizeof(Entry));
        }

        stream.pad(Alignment);

        for (auto&& data : m_data) {
            reinterpret_cast<Derived const*>(this)->serialize_data(stream, data);
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