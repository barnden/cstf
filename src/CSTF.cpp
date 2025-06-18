#include "CSTF.h"
#include <fstream>
#include <string>

using namespace cstf;

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

    CSTF cstf {};
    {
        auto file = std::ifstream(cstf_read_path, std::ios::binary);

        if (!file.is_open()) {
            std::println("Failed to open \"{}\" for reading.", cstf_read_path);
            exit(0);
        }

        cstf.deserialize(file);

        std::println("{}", cstf);
    }

    {
        auto file = std::ofstream(cstf_write_path, std::ios::binary);

        if (!file.is_open()) {
            std::println("Failed to open \"{}\" for writing.", cstf_read_path);
            exit(0);
        }

        cstf.serialize(file);
    }
}
