#pragma once

#include "CSTF.h"
#include "Serialize/Serializer.h"

namespace cstf::serialize {

template <>
struct Serializer<CSTF> : public BaseSerializer {
    void visit(CSTF const& cstf) const
    {
        cstf.m_header.accept(to<BaseSerializer>());
        cstf.m_game_data.accept(to<BaseSerializer>());
        cstf.m_rounds.accept(to<BaseSerializer>());
    }
};

template <>
struct Deserializer<CSTF> : public BaseDeserializer {
    void visit(CSTF& cstf) const
    {
        cstf.m_header.accept(to<BaseDeserializer>());
        cstf.m_game_data.accept(to<BaseDeserializer>());
        cstf.m_rounds.accept(to<BaseDeserializer>());
    }
};

}