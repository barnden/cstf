/*
 * Reads a CSTF file and optionally writes it back out.
 * Also checks if the serialization/deserialization reads/writes the same bytes.
 */

#include <CSTF.h>

#include <fstream>
#include <print>
#include <string>
#include <variant>

#include "common/StreamEquality.h"

using namespace cstf;
using namespace cstf::serialize;

auto main(int argc, char** argv) -> int
{
    std::string cstf_read_path = "data/example.cstf";
    std::optional<std::string> cstf_write_path = std::nullopt;

    if (argc >= 2) {
        cstf_read_path = argv[1];
    }

    if (argc >= 3) {
        cstf_write_path = argv[2];
    }

    CSTF cstf {};

    stream_t input = std::fstream(
        cstf_read_path,
        std::ios::in | std::ios::binary);

    stream_t output;

    if (auto* istream = std::get_if<std::fstream>(&input)) {
        if (!istream->is_open()) {
            std::println(
                "Failed to open \"{}\" for reading.",
                cstf_read_path);

            exit(0);
        }
    }

    if (cstf_write_path.has_value()) {
        auto file = std::fstream(
            *cstf_write_path,
            std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

        if (!file.is_open()) {
            std::println(
                "Failed to open \"{}\" for writing.",
                *cstf_write_path);

            exit(0);
        }

        output = std::move(file);
    } else {
        output = std::stringstream {};
    }

    std::visit(
        [&](auto& stream) {
            BaseDeserializer deserializer(stream);

            // Example: Read Round/Event LUT entries, but not the data
            // deserializer.options()
            //     .set(BaseDeserializer::Flags::ReadEventData, false);

            deserializer(cstf);
        },
        input);

    std::println("{}", cstf);

    std::visit(
        [&](auto& stream) {
            BaseSerializer serializer(stream);

            serializer(cstf);
        },
        output);

    // FIXME: Implement operator==() for CSTF related structs.
    //        Due to the flexibility of the CSTF format, byte-wise comparison
    //        of the streams is not the best way to determine equality.
    auto identical = is_equal(input, output);

    std::println(
        "CSTF file deserialization and reserialization produced {} results.",
        (identical ? "identical" : "different"));

    return identical ? EXIT_SUCCESS : EXIT_FAILURE;
}
