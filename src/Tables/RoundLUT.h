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

public:
    RoundLUT() = default;

    void accept(BaseDeserializer const& visitor)
    {
        LookupTable<RoundLUT, RoundLUTEntry, EventLUT>::accept(visitor);
    }

    void accept(BaseSerializer const& visitor)
    {
        LookupTable<RoundLUT, RoundLUTEntry, EventLUT>::accept(visitor);
    }

    void accept(BaseDeserializer const& visitor, RoundLUTEntry entry, size_t base)
    {
        // FIXME: We should allow for events in pauses, e.g. player purchase/join/etc
        if (entry.type != RoundLUTEntry::Type::ROUND)
            return;

        size_t position = base + m_offset_size * entry.offset;
        visitor.stream()->seekg(position);

        m_data.emplace_back();
        m_data.back().accept(visitor);
    }
};

namespace serialize {

    template<>
    struct Serializer<RoundLUT> : BaseSerializer {
        void visit(RoundLUT const& lut)  const{
            std::println("hello, world");
        }
    };

    // template <>
    // struct Deserializer<RoundLUT> : BaseSerializer {
    // };

}

};