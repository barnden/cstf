/*
 * Reads a CSTF file and optionally writes it back out.
 * Also checks if the serialization/deserialization reads/writes the same bytes.
 */

#include <CSTF.h>

#include <fstream>
#include <print>
#include <sstream>
#include <string>
#include <variant>

using namespace cstf;
using namespace cstf::serialize;

[[noreturn]] void check_if_equal(auto& input, auto& output)
{
    bool identical = false;

    using iterator_t = std::istreambuf_iterator<char>;
    input.seekg(std::ios_base::beg);
    iterator_t begin1 { input };
    iterator_t begin2 {};

    std::visit(
        [&](auto& stream) {
            stream.seekg(std::ios_base::beg);
            begin2 = { stream };
        },
        output);

    identical = std::equal(begin1, iterator_t {}, begin2);
    std::print("CSTF file serialization and deserialization produced");

    if (identical) {
        std::println(" identical results.");
        exit(0);
    } else {
        std::println(" different results.");
        exit(1);
    }
}

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

    auto input = std::ifstream(cstf_read_path, std::ios::binary);
    std::variant<std::stringstream, std::fstream> output;

    if (!input.is_open()) {
        std::println("Failed to open \"{}\" for reading.", cstf_read_path);
        exit(0);
    }

    if (cstf_write_path.has_value()) {
        auto file = std::fstream(*cstf_write_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

        if (!file.is_open()) {
            std::println("Failed to open \"{}\" for writing.", *cstf_write_path);
            exit(0);
        }

        output = std::move(file);
    }

    {
        BaseDeserializer deserializer(input);

        // Example: Read round and event LUT entries, but not the actual event data
        // deserializer.options().set(BaseDeserializer::Flags::ReadEventData, 0);

        deserializer(cstf);

        std::println("{}", cstf);
    }

    std::visit(
        [&](auto& stream) {
            BaseSerializer serializer(stream);

            serializer(cstf);
        },
        output);

    check_if_equal(input, output);
}
