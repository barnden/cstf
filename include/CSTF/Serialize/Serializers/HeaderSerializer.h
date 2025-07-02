#pragma once

#include "CSTF/Format/Header.h"
#include "CSTF/Serialize/Serializer.h"

#include <bit>

namespace cstf::serialize {
template <>
struct Serializer<Header> : BaseSerializer {
    void visit(Header const& header) const
    {
        m_stream->write(std::bit_cast<char const*>(&header.m_cstf_magic), sizeof(header.m_cstf_magic));

        u8 flags = static_cast<u8>(header.m_flags.to_ulong());
        m_stream->write(reinterpret_cast<char const*>(&flags), 1);

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

        u8 flags = 0;
        m_stream->read(reinterpret_cast<char*>(&flags), 1);

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