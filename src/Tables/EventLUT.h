#pragma once

#include "Types.h"
#include "Tables/LookupTable.h"
#include "Events/Events.h"

namespace CSTF {

#pragma pack(1)
struct EventLUTEntry {
    u32 offset : 20 {};
    u32 next : 18 {};
    u8 frames : 4 {};
    u8 type : 6 {};

    [[nodiscard]] auto to_string() const -> std::string
    {
        return std::format("EventLUTEntry(type: {:06b}, frames: {}, next: {:04x} offset: {:06x})",
                           type, frames, next, offset);
    }
};
#pragma pack()

static_assert(sizeof(EventLUTEntry) == 6);

struct EventLUT : public LookupTable<EventLUTEntry> {
    // FIXME (1): vector of variants makes me sad ;(
    std::vector<EventTypes::variant_t> events {};

    EventLUT() = default;
    EventLUT(Stream stream)
        : LookupTable<EventLUTEntry>(stream)
    {
        events.reserve(m_entries.size());

        stream.consume_padding<4>();
        size_t start = stream->tellg();

        for (auto&& entry : m_entries) {
            u32 offset = entry.offset;
            auto position = start + 2 * offset;
            stream->seekg(position);

            for_sequence<2>([&](auto i) {
                using Event = EventTypes::get<i>;

                if (entry.type != i)
                    return;

                auto event = Event {};
                stream->read(reinterpret_cast<char*>(&event), sizeof(Event));

                events.push_back(event);
            });
        }
    }
};
};