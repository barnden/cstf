#pragma once

#include "CSTF.h"
#include <span>
#include <vector>

namespace CSTF {

#pragma pack(1)
struct CSTFRoundLUTEntry {
    u32 offset : 24 {};
    u32 frame_offset : 22 {};
    u8 type : 2 {};

    [[nodiscard]] auto to_string() const -> std::string
    {
        return std::format("CSTFRoundLUTEntry(type: {:02b}, frame_offset: {:03x}, offset: {:03x})", type, frame_offset, offset);
    }
};
#pragma pack()

static_assert(sizeof(CSTFRoundLUTEntry) == 6);

struct CSTFRoundLUT {
    std::vector<CSTFRoundLUTEntry> entries;

    CSTFRoundLUT() = delete;
    CSTFRoundLUT(Stream stream)
    {
        stream.consume_padding<4>();
        u32 num_bytes = 0;
        stream->read(reinterpret_cast<char*>(&num_bytes), 4);

        auto size = num_bytes / sizeof(CSTFRoundLUTEntry);

        entries = std::vector<CSTFRoundLUTEntry>(size);
        stream->read(reinterpret_cast<char*>(entries.data()), num_bytes);
    }

    [[nodiscard]] auto to_string() const -> std::string
    {
        return std::format("CSTFRoundLUT(size: {})", entries.size());
    }
};

};