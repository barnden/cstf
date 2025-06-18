#include "CSTF.h"
#include <fstream>
#include <string>

auto main(int argc, char** argv) -> int
{
    std::string cstf_filepath = "data/example.cstf";

    if (argc == 2) {
        cstf_filepath = argv[1];
    }

    CSTF::CSTF cstf {};
    {
        auto file = std::ifstream(cstf_filepath, std::ios::binary);
        cstf.deserialize(file);

        std::println("{}", cstf);
    }

    {
        auto file = std::ofstream("../data/write.cstf", std::ios::binary);
        cstf.serialize(file);
    }

    {
        auto file = std::ifstream("../data/write.cstf", std::ios::binary);
        CSTF::Header header{};
        CSTF::GameData game_data{};

        header.deserialize(file);
        game_data.deserialize(file);

        std::println("{}", header);
        std::println("{}", game_data);
    }
}
