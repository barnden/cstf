#pragma once

#include "CSTF/Serialize/Serializable.h"
#include "CSTF/Utility/Types.h"

#include <vector>

namespace cstf {

using namespace serialize;

namespace serialize {
    struct LUTDeserializer;
}

template <class Derived, class Entry, class Data, size_t Alignment = 4>
class LookupTable : IStringable<LookupTable<Derived, Entry, Data, Alignment>>,
                    public Serializable<LookupTable<Derived, Entry, Data, Alignment>> {
    friend Serializer<LookupTable<Derived, Entry, Data, Alignment>>;
    friend Deserializer<LookupTable<Derived, Entry, Data, Alignment>>;

protected:
    std::vector<Entry> m_entries {};
    std::vector<Data> m_data {};
    u32 m_offset_size = 4;

public:
    using derived_type = Derived;
    using entry_type = Entry;
    using data_type = Data;
    static constexpr size_t alignment = Alignment;

    LookupTable() = default;
    virtual ~LookupTable() = default;

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format("LookupTable<{}>(size: {})", demangle(typeid(Entry)), m_entries.size());
    }

    auto entries() const -> std::vector<Entry> const&
    {
        return m_entries;
    }

    auto data() const -> std::vector<Data> const&
    {
        return m_data;
    }
};

};