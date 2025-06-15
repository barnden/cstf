#include <cstring>
#include <fstream>
#include <print>

#include "CSTFGameData.h"
#include "CSTFHeader.h"
#include "CSTFRoundLUT.h"

using namespace CSTF;

auto main() -> int
{
    auto file = std::ifstream("example.cstf", std::ios::binary);
    auto header = CSTFHeader(file);
    auto game_data = CSTFGameData(file);
    auto round_LUT = CSTFRoundLUT(file);

    std::println("{} is_valid: {}", header, header.is_valid());
    std::println("{}", game_data);
    std::println("{}", round_LUT);

    for (auto&& entry : round_LUT.entries) {
        std::println("\t{}", entry);
    }
    std::println("{:016x} {}", 76561198065277623, SteamID(76561198065277623));
    std::println("{:016x} {}", 76561198039184405, SteamID(76561198039184405));
}
