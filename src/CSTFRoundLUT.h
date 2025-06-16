#pragma once

#include "CSTF.h"
#include "CSTFLookupTable.h"

namespace CSTF {

#pragma pack(1)
struct CSTFRoundLUTEntry {
    u32 offset : 24 {};
    u32 frame_offset : 22 {};
    u8 type : 2 {};

    [[nodiscard]] auto to_string() const -> std::string
    {
        return std::format("CSTFRoundLUTEntry(type: {:02b}, frame_offset: {:06x}, offset: {:06x})",
                           type, frame_offset, offset);
    }
};
#pragma pack()

static_assert(sizeof(CSTFRoundLUTEntry) == 6);

using CSTFRoundLUT = CSTFLookupTable<CSTFRoundLUTEntry>;

};