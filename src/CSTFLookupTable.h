#pragma once

#include "CSTF.h"

#include <cxxabi.h>
#include <vector>

namespace CSTF {

template <class Entry, size_t Alignment = 4>
struct CSTFLookupTable {
    std::vector<Entry> entries {};

    CSTFLookupTable() = default;
    CSTFLookupTable(Stream stream)
    {
        stream.consume_padding<Alignment>();

        u32 num_bytes = 0;
        stream->read(reinterpret_cast<char*>(&num_bytes), 4);

        auto size = num_bytes / sizeof(Entry);
        entries = std::vector<Entry>(size);

        stream->read(reinterpret_cast<char*>(entries.data()), num_bytes);
    }

    virtual ~CSTFLookupTable() { };

    [[nodiscard]] virtual auto to_string() const -> std::string
    {
        int status {};

        char* type = abi::__cxa_demangle(typeid(Entry).name(), NULL, NULL, &status);
        std::string result = std::format("CSTFLookupTable<{}>(size: {})", type, entries.size());
        free(type);

        return result;
    }
};

};