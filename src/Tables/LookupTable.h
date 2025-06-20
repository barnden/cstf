#pragma once

#include "Serializable.h"
#include "Types.h"

#include <cxxabi.h>
#include <print>
#include <vector>

namespace cstf {

using namespace serialize;

template <class Derived, class Entry, class Data, size_t Alignment = 4>
class LookupTable : IStringable<LookupTable<Derived, Entry, Data, Alignment>>,
                    public Serializable<LookupTable<Derived, Entry, Data, Alignment>> {

    friend Serializer<LookupTable<Derived, Entry, Data, Alignment>>;
    friend Deserializer<LookupTable<Derived, Entry, Data, Alignment>>;

protected:
    std::vector<Entry> m_entries {};
    std::vector<Data> m_data {};
    u32 m_offset_size = 4;

    // void accept(
    //     BaseDeserializer const& deserializer,
    //     Entry const& entry,
    //     size_t base)
    // {
    //     size_t position = base + m_offset_size * entry.offset;
    //     deserializer.m_stream->seekg(position);

    //     m_data.emplace_back();
    //     m_data.back().accept(deserializer);
    // }

    // void serialize_data(ostream const& stream, Data const& data) const
    // {
    //     data.serialize(stream);
    // }

public:
    using derived_type = Derived;
    using entry_type = Entry;
    using data_type = Data;
    static constexpr size_t alignment = Alignment;

    LookupTable() = default;
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

    auto data() const -> std::vector<Data> const&
    {
        return m_data;
    }

    virtual void add(Entry& entry)
    {
        m_entries.push_back(entry);
    }
};

namespace serialize {
    template <class Derived, class Entry, class Data, size_t Alignment>
    struct Serializer<LookupTable<Derived, Entry, Data, Alignment>> : BaseSerializer {
        void visit(LookupTable<Derived, Entry, Data, Alignment> const& lut) const
        {
            m_stream.pad(Alignment);

            u32 num_bytes = lut.m_entries.size() * sizeof(Entry);
            m_stream->write(reinterpret_cast<char const*>(&num_bytes), 4);

            for (auto&& entry : lut.m_entries) {
                m_stream->write(reinterpret_cast<char const*>(&entry), sizeof(Entry));
            }

            m_stream.pad(Alignment);

            for (auto const&& data : lut.m_data) {
                data.accept(to<BaseSerializer>());
                static_cast<Derived&>(lut).accept(to<BaseSerializer>());
            }
        }
    };

    template <class Derived, class Entry, class Data, size_t Alignment>
    struct Deserializer<LookupTable<Derived, Entry, Data, Alignment>> : BaseDeserializer {
        void visit(LookupTable<Derived, Entry, Data, Alignment>& lut) const
        {
            m_stream.consume_padding(Alignment);

            u32 num_bytes = 0;
            m_stream->read(reinterpret_cast<char*>(&num_bytes), 4);

            auto size = num_bytes / sizeof(Entry);
            lut.m_entries = std::vector<Entry>(size);

            for (auto&& entry : lut.m_entries) {
                entry.accept(to<Entry>());
            }

            m_stream.consume_padding(Alignment);

            size_t base = m_stream->tellg();
            for (auto&& entry : lut.m_entries) {
                static_cast<Derived&>(lut).accept(to<BaseDeserializer>(), entry, base);
            }
        }
    };

}

};