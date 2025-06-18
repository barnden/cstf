#include "CSTF.h"
#include <fstream>
#include <string>

auto main(int argc, char** argv) -> int
{
    std::string cstf_read_path = "data/example.cstf";
    std::string cstf_write_path = "data/out.cstf";

    if (argc == 2) {
        cstf_read_path = argv[1];
    }

    if (argc == 3) {
        cstf_write_path = argv[2];
    }

    CSTF::CSTF cstf {};
    {
        auto file = std::ifstream(cstf_read_path, std::ios::binary);
        cstf.deserialize(file);
    }

    {
        auto file = std::ofstream(cstf_write_path, std::ios::binary);
        cstf.serialize(file);
    }
}
