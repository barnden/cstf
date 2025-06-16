#include <cstring>
#include <fstream>
#include <print>
#include <vector>

#include "CSTFGameData.h"
#include "CSTFHeader.h"
#include "CSTFRoundEventLUT.h"
#include "CSTFRoundLUT.h"

using namespace CSTF;

auto main() -> int
{
    auto file = std::ifstream("data/example.cstf", std::ios::binary);
    auto header = CSTFHeader(file);
    auto game_data = CSTFGameData(file);
    auto round_LUT = CSTFRoundLUT(file);

    std::println("{} is_valid: {}", header, header.is_valid());
    std::println("{}", game_data);

    std::println("{}", round_LUT);

    for (auto&& entry : round_LUT.entries) {
        std::println("\t{}", entry);
    }

    // TODO: Move this logic elsewhere
    auto round_event_LUTs = std::vector<CSTFRoundEventLUT> {};
    Stream stream(file);
    stream.consume_padding<4>();
    size_t round_event_start = file.tellg();

    for (auto&& entry : round_LUT.entries) {
        if (entry.type != 1)
            continue;

        file.seekg(round_event_start + entry.offset * 4);
        round_event_LUTs.emplace_back(file);

        auto const& lut = round_event_LUTs.back();

        std::println("{}", lut);

        for (auto const&& [event_entry, event] : std::views::zip(lut.entries, lut.events)) {
            std::println("\t{}", event_entry);

            std::visit(
                [](const auto& req) {
                    std::println("\t\t{}", req);
                },
                event);
        }
    }
}
