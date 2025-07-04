#pragma once

#include <fstream>
#include <sstream>
#include <variant>

using stream_t = std::variant<std::stringstream, std::fstream>;

// clang-format off
template <typename T>
concept Streamlike =
    requires(T x) {
        { x.seekg(std::ios_base::beg) };
    } &&
    requires(T x) {
        { std::istreambuf_iterator<char> { x }
    };
};
// clang-format on

template <Streamlike IStream, Streamlike OStream>
[[nodiscard]] auto is_equal(IStream& input, OStream& output)
{
    if (!input || !output)
        return false;

    using iterator_t = std::istreambuf_iterator<char>;

    input.seekg(std::ios_base::beg);
    output.seekg(std::ios_base::beg);

    iterator_t begin1 { input };
    iterator_t begin2 { output };

    return std::equal(begin1, {}, begin2);
}

[[nodiscard]] auto is_equal(stream_t& input_v, stream_t& output_v)
{
    auto result = false;

    std::visit(
        [&output_v, &result](auto& istream) {
            std::visit(
                [&istream, &result](auto& ostream) {
                    result = is_equal(istream, ostream);
                },
                output_v);
        },
        input_v);

    return result;
}
