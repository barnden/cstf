#pragma once

#include <print>
#include <variant>
#include <vector>

#include "GameData.h"
#include "Header.h"
#include "Serializable.h"
#include "Tables/EventLUT.h"
#include "Tables/RoundLUT.h"
#include "Types.h"

namespace cstf {

using namespace cstf::serialize;

class CSTF : public IStringable<CSTF>, public Serializable<CSTF> {
    Header m_header {};
    GameData m_game_data {};
    RoundLUT m_rounds {};

public:
    CSTF() = default;

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        using std::views::zip;

        std::string result;
        auto cur_round = 0;

        std::println("{}", m_header);
        std::println("{}", m_game_data);

        for (auto&& round : m_rounds.entries()) {
            std::println("{}", round);

            if (round.type != RoundLUTEntry::Type::ROUND)
                continue;

            auto const& events = m_rounds.data()[cur_round++];
            for (auto const&& [event, data] : zip(events.entries(), events.data())) {
                std::println("\t{}", event);
                std::visit(
                    [](auto const& e) { std::println("\t\t{}", e); },
                    data);
            }
        }

        return result;
    }

    void accept(auto const& visitor)
    {
        m_header.accept(visitor);
        m_game_data.accept(visitor);
        m_rounds.accept(visitor);
    }
};

}