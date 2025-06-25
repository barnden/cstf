#include "CSTF.h"
#include "GameData.h"
#include "Serialize/Serialize.h"

#include <fstream>
#include <print>
#include <string>

using namespace cstf;
using namespace cstf::serialize;

auto main(int argc, char** argv) -> int
{
    std::string cstf_read_path = "data/example.cstf";
    std::string cstf_write_path = "data/out.cstf";

    if (argc >= 2) {
        cstf_read_path = argv[1];
    }

    if (argc >= 3) {
        cstf_write_path = argv[2];
    }

    CSTF cstf {};
    {
        auto file = std::ifstream(cstf_read_path, std::ios::binary);
        BaseDeserializer deserializer(file);

        if (!file.is_open()) {
            std::println("Failed to open \"{}\" for reading.", cstf_read_path);
            exit(0);
        }

        cstf.accept(deserializer);

        std::println("{}", cstf);
    }

    {
        auto file = std::ofstream(cstf_write_path, std::ios::binary);
        BaseSerializer serializer(file);

        if (!file.is_open()) {
            std::println("Failed to open \"{}\" for writing.", cstf_read_path);
            exit(0);
        }

        cstf.accept(serializer);
    }
}
