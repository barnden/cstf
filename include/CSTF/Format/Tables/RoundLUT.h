#pragma once

#include "CSTF/Format/Tables/EventLUT.h"
#include "CSTF/Format/Tables/LookupTable.h"
#include "CSTF/Utility/Types.h"

namespace cstf {

using namespace serialize;

#pragma pack(1)
struct RoundLUTEntry : IStringable<RoundLUTEntry>,
                       public Serializable<RoundLUTEntry> {
    u32 offset : 24 {};
    u32 frame_offset : 22 {};

    enum Type {
        INVALID,
        ROUND,
        PAUSE,
        PADDING
    } type : 2 {};

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format("RoundLUTEntry(type: {:02b}, frame_offset: {:06x}, offset: {:06x})",
                           (u8)type, frame_offset, offset);
    }
};
#pragma pack()

static_assert(sizeof(RoundLUTEntry) == 6);

class RoundLUT : public LookupTable<RoundLUT, RoundLUTEntry, EventLUT> {
    friend LookupTable<RoundLUT, RoundLUTEntry, EventLUT>;
    friend Deserializer<RoundLUT>;

    u32 m_table_size {};
    u32 m_last_frame {};

public:
    RoundLUT() = default;

    void add(RoundLUTEntry::Type type, u32 frame)
    {
        RoundLUTEntry entry {
            .offset = m_table_size / m_offset_size,
            .frame_offset = std::max(m_last_frame, frame) - m_last_frame,
            .type = type,
        };

        m_last_frame = std::max(m_last_frame, frame);

        m_entries.push_back(std::move(entry));
    }

    void add(RoundLUTEntry::Type type, u32 frame, EventLUT&& events)
    {
        add(type, frame);

        m_table_size += events.size();

        m_data.push_back(std::move(events));
    }
};

};