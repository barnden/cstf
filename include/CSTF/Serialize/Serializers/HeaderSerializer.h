#pragma once

#include "CSTF/Header.h"
#include "CSTF/Serialize/Serializer.h"

#include <bit>

namespace cstf::serialize {
template <>
struct Serializer<Header> : BaseSerializer {
    void visit(Header const& header) const
    {
        m_stream->write(std::bit_cast<char const*>(&header.cstf_magic), sizeof(header.cstf_magic));

        m_stream << header.flags;

        m_stream->write(std::bit_cast<char const*>(&header.reserved), 2);

        m_stream << header.version << header.tick_rate;

        m_stream->write(header.map_name.c_str(), header.map_name.size() + 1);

        m_stream->write(std::bit_cast<char const*>(&header.build_info), 4);
    }
};

template <>
struct Deserializer<Header> : public BaseDeserializer {
    void visit(Header& header) const
    {
        m_stream->read(std::bit_cast<char*>(&header.cstf_magic), sizeof(header.cstf_magic));

        m_stream >> header.flags;

        m_stream->read(std::bit_cast<char*>(&header.reserved), 2);

        m_stream >> header.version >> header.tick_rate;

        std::getline(*m_stream, header.map_name, '\0');

        m_stream->read(std::bit_cast<char*>(&header.build_info), 4);
    }
};
}