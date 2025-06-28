#pragma once

#include "CSTF/Events/Events.h"
#include "CSTF/Serialize/Serializer.h"
#include "CSTF/Tables/EventLUT.h"
#include "CSTF/Tables/LookupTable.h"
#include "CSTF/Tables/RoundLUT.h"

#include <bit>

namespace cstf::serialize {

template <class Derived, class Entry, class Data, size_t Alignment>
struct Serializer<LookupTable<Derived, Entry, Data, Alignment>> : BaseSerializer {
    void visit(LookupTable<Derived, Entry, Data, Alignment> const& lut) const
    {
        m_stream.pad(Alignment);

        u32 num_bytes = lut.m_entries.size() * sizeof(Entry);
        m_stream->write(std::bit_cast<char const*>(&num_bytes), 4);

        for (auto&& entry : lut.m_entries) {
            m_stream->write(std::bit_cast<char const*>(&entry), sizeof(Entry));
        }

        m_stream.pad(Alignment);

        for (auto&& [entry, data] : std::views::zip(lut.m_entries, lut.m_data)) {
            to<Derived>().visit(static_cast<Derived const&>(lut), entry, data);
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
            to<Derived>().visit(static_cast<Derived&>(lut), entry, base);
        }
    }
};

template <>
struct Serializer<EventLUT> : Serializer<LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t>> {
    void visit(EventLUT const&) const { ASSERT_NOT_REACHED; }

    void visit(EventLUT const&, EventLUTEntry const&, auto const& data) const
    {
        std::visit(
            [&](auto const& event) {
                event.accept(to<BaseSerializer>());
            },
            data);
    }
};

template <>
struct Deserializer<EventLUT> : Deserializer<LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t>> {
    void visit(EventLUT&) const { ASSERT_NOT_REACHED; }

    void visit(EventLUT& lut, EventLUTEntry entry, size_t base) const
    {
        // FIXME: We shouldn't dispatch in the first place if this flag is unset.
        if (!m_flags[Flags::ReadEventData])
            return;

        size_t position = base + lut.m_offset_size * entry.offset;
        m_stream->seekg(position);

        for_sequence<EventTypes::size>(
            [&](auto i) {
                using EventType = EventTypes::get<i>;

                if (entry.type != i)
                    return;

                EventType event {};
                event.accept(to<BaseDeserializer>());
                lut.m_data.push_back(event);
            });
    }
};

template <>
struct Serializer<RoundLUT> : Serializer<LookupTable<RoundLUT, RoundLUTEntry, EventLUT>> {
    void visit(RoundLUT&) const { ASSERT_NOT_REACHED; }

    void visit(RoundLUT const&, RoundLUTEntry const&, auto const& data) const
    {
        data.accept(to<BaseSerializer>());
    }
};

template <>
struct Deserializer<RoundLUT> : Deserializer<LookupTable<RoundLUT, RoundLUTEntry, EventLUT>> {
    void visit(RoundLUT&) const { ASSERT_NOT_REACHED; }

    void visit(RoundLUT& lut, RoundLUTEntry entry, size_t base) const
    {
        // FIXME: We should allow for events in pauses, e.g. player purchase/join/etc
        if (entry.type != RoundLUTEntry::Type::ROUND)
            return;

        size_t position = base + lut.m_offset_size * entry.offset;
        m_stream->seekg(position);

        if (m_flags[Flags::ReadEventLUT]) {
            lut.m_data.emplace_back();
            lut.m_data.back().accept(to<BaseDeserializer>());
        }
    }
};

}