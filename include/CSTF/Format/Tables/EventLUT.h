#pragma once

#include "CSTF/Format/Events/Events.h"
#include "CSTF/Format/Tables/LookupTable.h"
#include "CSTF/Utility/Types.h"

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

    u32 m_table_size {};
    u32 m_last_frame {};
    std::array<std::optional<u32>, EventTypes::size> m_events {};

public:
    EventLUT()
        : LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t>()
    {
        m_offset_size = 2;
    };

    template <typename T>
        requires(std::is_base_of_v<Event<T>, T>)
    void add(T&& event, u32 frame = 0)
    {
        // Assumption is that frame is monotonically increasing.

        EventLUTEntry entry {
            .offset = m_table_size / m_offset_size,
            .next = 0x3FFFF,
            .frames = std::max(m_last_frame, frame) - m_last_frame,
            .type = EventTypes::index<T>()
        };

        auto& last_event = m_events[EventTypes::index<T>()];

        if (last_event.has_value()) {
            m_entries[*last_event].next = m_entries.size() - *last_event;
        }

        last_event = m_entries.size();
        m_last_frame = std::max(m_last_frame, frame);
        m_table_size += sizeof(T);

        m_entries.push_back(entry);
        m_data.push_back(std::move(event));
    }

    [[nodiscard]] constexpr auto size() const noexcept
    {
        // 4 bytes for the size of the table entry sizes
        return 4 + m_table_size;
    }

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        using std::views::zip;

        auto result = LookupTable<EventLUT, EventLUTEntry, EventTypes::variant_t>::to_string();

        if (m_entries.size() == 0)
            return result;

        result += '\n';

        if (m_entries.size() == m_data.size()) {
            for (auto const&& [event, data] : zip(m_entries, m_data)) {
                result += std::format("\t{}\n", event);
                std::visit(
                    [&result](auto const& e) {
                        result += std::format("\t\t{}\n", e);
                    },
                    data);
            }
        } else {
            for (auto&& event : m_entries) {
                result += std::format("\t{}\n", event);
            }
        }

        return result;
    }
};

};