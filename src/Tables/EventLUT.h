#pragma once

#include "Events/BaseEvent.h"
#include "Events/Events.h"
#include "Tables/LookupTable.h"
#include "Types.h"

namespace CSTF {

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

struct EventLUT : public LookupTable<EventLUTEntry> {
    std::vector<EventTypes::variant_t> events {};

    EventLUT() = default;
    EventLUT(istream stream)
        : LookupTable<EventLUTEntry>(stream)
    {
        events.reserve(m_entries.size());

        stream.consume_padding(4);
        size_t start = stream->tellg();

        for (auto&& entry : m_entries) {
            u32 offset = entry.offset;
            auto position = start + 2 * offset;
            stream->seekg(position);

            for_sequence<2>(
                [&](auto i) {
                    using EventType = EventTypes::get<i>;

                    if (entry.type != i)
                        return;

                    events.push_back(std::move(EventType::from(stream)));
                });
        }
    }
};
};