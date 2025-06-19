#pragma once

#include "Events/Events.h"
#include "Tables/LookupTable.h"
#include "Types.h"

#include <Serializable.h>

namespace cstf {

using namespace serialize;

#pragma pack(1)
struct EventLUTEntry : public IStringable<EventLUTEntry>,
                       public Serializable<EventLUTEntry> {
    u32 offset : 20 {};
    u32 next : 18 {};
    u8 frames : 4 {};
    u8 type : 6 {};

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format("EventLUTEntry(type: {:06b}, frames: {}, next: {:04x} offset: {:06x})",
                           type, frames, next, offset);
    }
};
#pragma pack()

static_assert(sizeof(EventLUTEntry) == 6);

class EventLUT : public LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t> {
    friend LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t>;
    friend Deserializer<EventLUT>;

public:
    EventLUT()
        : LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t>()
    {
        m_offset_size = 2;
    };
};

namespace serialize {
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
};

};