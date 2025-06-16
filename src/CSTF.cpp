#include "CSTF.h"
#include <string>
#include <fstream>

auto main(int argc, char** argv) -> int
{
    std::string cstf_filepath = "data/example.cstf";

    if (argc == 2) {
        cstf_filepath = argv[1];
    }

    auto file = std::ifstream(cstf_filepath, std::ios::binary);
    auto cstf = CSTF::CSTF(file);

    std::println("{}", cstf);
}
