#pragma once

#include <variant>
#include <vector>

#include "CSTF/Format/GameData.h"
#include "CSTF/Format/Header.h"
#include "CSTF/Format/Tables/EventLUT.h"
#include "CSTF/Format/Tables/RoundLUT.h"
#include "CSTF/Serialize/Serializable.h"
#include "CSTF/Utility/Types.h"

namespace cstf {

using namespace cstf::serialize;

class CSTF : public IStringable<CSTF>, public Serializable<CSTF> {
    Header m_header {};
    GameData m_game_data {};
    RoundLUT m_rounds {};

    friend Serializer<CSTF>;
    friend Deserializer<CSTF>;

public:
    CSTF() = default;

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {

        std::string result {};
        auto cur_round = 0;

        result += std::format("{}\n", m_header);
        result += std::format("{}\n", m_game_data);

        // FIXME: Move this logic to the appropriate to_string() methods in the respective classes
        for (auto&& round : m_rounds.entries()) {
            result += std::format("{}\n", round);

            if (round.type != RoundLUTEntry::Type::ROUND)
                continue;

            if (m_rounds.data().size() == 0)
                continue;

            auto const& events = m_rounds.data().at(cur_round++);

            result += std::format("{}\n", events);
        }

        return result;
    }
};

}