#pragma once

#include "Tables/EventLUT.h"
#include "Tables/LookupTable.h"
#include "Types.h"

namespace cstf {

#pragma pack(1)
struct RoundLUTEntry : IStringable<RoundLUTEntry> {
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

struct RoundLUT : public LookupTable<RoundLUTEntry, EventLUT> {
    RoundLUT() = default;

    void deserialize(istream const& stream)
    {
        LookupTable<RoundLUTEntry, EventLUT>::deserialize(stream);

        m_data.reserve(m_entries.size());
        stream.consume_padding(4);

        size_t event_base = stream->tellg();
        for (auto&& entry : m_entries) {
            // FIXME: We should probably allow events in non-rounds
            //        e.g. player purchasing a weapon in pause
            if (entry.type != RoundLUTEntry::Type::ROUND)
                continue;

            u32 offset = entry.offset;
            auto position = event_base + 4 * offset;

            stream->seekg(position);

            m_data.emplace_back();
            m_data.back().deserialize(stream);
        }
    }

    void serialize(ostream const& stream) const
    {
        LookupTable<RoundLUTEntry, EventLUT>::serialize(stream);

        stream.pad(4);
        for (auto&& lut : m_data) {
            lut.serialize(stream);
        }
    }
};

};