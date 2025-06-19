#pragma once

#include "Tables/EventLUT.h"
#include "Tables/LookupTable.h"
#include "Types.h"
#include <Serializable.h>

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

public:
    RoundLUT() = default;
};

namespace serialize {
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

            lut.m_data.emplace_back();
            lut.m_data.back().accept(to<BaseDeserializer>());
        }
    };
};

};