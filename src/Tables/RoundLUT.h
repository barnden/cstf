#pragma once

#include "Tables/LookupTable.h"
#include "Types.h"

namespace CSTF {

#pragma pack(1)
struct RoundLUTEntry {
    u32 offset : 24 {};
    u32 frame_offset : 22 {};

    enum Type {
        INVALID,
        ROUND,
        PAUSE,
        PADDING
    } type : 2 {};

    [[nodiscard]] auto to_string() const -> std::string
    {
        return std::format("RoundLUTEntry(type: {:02b}, frame_offset: {:06x}, offset: {:06x})",
                           (u8)type, frame_offset, offset);
    }
};
#pragma pack()

static_assert(sizeof(RoundLUTEntry) == 6);

using RoundLUT = LookupTable<RoundLUTEntry>;

};