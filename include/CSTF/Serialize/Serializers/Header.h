#pragma once

#include "CSTF/Format/Header.h"
#include "CSTF/Serialize/Serializer.h"

#include <cassert>
#include <bit>
#include <stdexcept>

namespace cstf::serialize {
template <>
struct Serializer<Header> : BaseSerializer {
    void visit(Header const& header) const
    {
        m_stream->write(std::bit_cast<char const*>(&header.m_cstf_magic), sizeof(header.m_cstf_magic));

        auto flags = header.m_flags.to_ulong();
        constexpr auto num_bytes = ((header.m_flags.size() + 7) & 0xfffffff8ul) >> 3;

        m_stream->write(reinterpret_cast<char const*>(&flags), num_bytes);

        m_stream->write(std::bit_cast<char const*>(&header.m_reserved), 2);

        m_stream->write(reinterpret_cast<char const*>(&header.m_version), 1);

        m_stream->write(reinterpret_cast<char const*>(&header.m_tick_rate), 1);

        m_stream->write(header.m_map_name.c_str(), header.m_map_name.size() + 1);

        m_stream->write(std::bit_cast<char const*>(&header.m_build_info), 4);
    }
};

template <>
struct Deserializer<Header> : public BaseDeserializer {
    void visit(Header& header) const
    {
        m_stream->read(std::bit_cast<char*>(&header.m_cstf_magic), sizeof(header.m_cstf_magic));

        if (!header.is_valid())
            throw std::runtime_error(
                std::format(
                    R"([Deserializer] Header magic "{::#02x}" does not match the expected "{::#02x}".)",
                    header.m_cstf_magic,
                    header.g_magic_bytes)
            );

        auto flags = 0ul;
        constexpr auto num_bytes = ((header.m_flags.size() + 7) & 0xfffffff8ul) >> 3;
        m_stream->read(reinterpret_cast<char*>(&flags), num_bytes);

        header.m_flags.reset();
        for (auto i = 0; flags != 0; i++, flags >>= 1) {
            header.m_flags.set(i, flags & 1);
        }

        m_stream->read(std::bit_cast<char*>(&header.m_reserved), 2);

        m_stream->read(reinterpret_cast<char*>(&header.m_version), 1);

        m_stream->read(reinterpret_cast<char*>(&header.m_tick_rate), 1);

        std::getline(*m_stream, header.m_map_name, '\0');

        m_stream->read(std::bit_cast<char*>(&header.m_build_info), 4);
    }
};
}