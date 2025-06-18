#pragma once

#include "Events/Events.h"
#include "Tables/LookupTable.h"
#include "Types.h"

#include <variant>

namespace cstf {

#pragma pack(1)
struct EventLUTEntry : IStringable<EventLUTEntry> {
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
    void deserialize_data(istream const& stream, EventLUTEntry const& entry, size_t base)
    {
        size_t position = base + m_offset_size * entry.offset;
        stream->seekg(position);

        for_sequence<EventTypes::size>(
            [&](auto i) {
                using EventType = EventTypes::get<i>;

                if (entry.type != i)
                    return;

                m_data.push_back(std::move(EventType::from(stream)));
            });
    }

    void serialize_data(ostream const& stream, EventTypes::variant_t const& data) const
    {
        std::visit(
            [&stream](auto const& event) {
                event.serialize(stream);
            },
            data);
    }

    friend LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t>;

public:
    EventLUT()
        : LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t>()
    {
        m_offset_size = 2;
    };
};
};