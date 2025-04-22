#include "Literal2Cpp.hpp"
#include <charconv>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <system_error>

namespace sonnx
{

auto Literal2Cpp::integerLiteral2CppInteger(const std::string &integer_literal) -> std::variant<uint32_t, uint64_t>
{
    bool has_suffix = false;
    std::string_view numeric_part{};
    if (integer_literal.back() == 'L' || integer_literal.back() == 'l')
    {
        has_suffix = true;
        numeric_part = std::string_view(integer_literal).substr(0, integer_literal.size() - 1);
    }
    else
    {
        numeric_part = std::string_view(integer_literal);
    }
    if (has_suffix)
    {
        uint64_t value = 0;
        auto [ptr, ec] = std::from_chars(numeric_part.data(), numeric_part.data() + numeric_part.size(), value);
        if (ec != std::errc())
        {
            if (ec == std::errc::result_out_of_range)
            {
                throw std::out_of_range("Integer literal out of uint64_t range");
            }
        }
        return value; // uint64_t
    }
    uint32_t value = 0;
    auto [ptr, ec] = std::from_chars(numeric_part.data(), numeric_part.data() + numeric_part.size(), value);
    if (ec != std::errc())
    {
        if (ec == std::errc::result_out_of_range)
        {
            throw std::out_of_range("Integer literal out of uint32_t range");
        }
    }
    return value; // uint32_t
}

auto Literal2Cpp::stringLiteral2CppString(const std::string &string_literal) noexcept(true) -> std::string
{
    std::string result{};
    result.reserve(string_literal.size() - 2);
    bool in_escape_sequence = false;
    for (char character : string_literal.substr(1, string_literal.size() - 2))
    {
        if (in_escape_sequence)
        {
            switch (character)
            {
            case 'b':
                result += '\b';
                break;
            case 't':
                result += '\t';
                break;
            case 'n':
                result += '\n';
                break;
            case 'f':
                result += '\f';
                break;
            case 'r':
                result += '\r';
                break;
            case '\'':
                result += '\'';
                break;
            case '\"':
                result += '\"';
                break;
            case '\\':
                result += '\\';
                break;
            default:
                break;
            }
            in_escape_sequence = false;
        }
        else
        {
            if (character == '\\')
            {
                in_escape_sequence = true;
            }
            else
            {
                result += character;
            }
        }
    }
    return result;
}

auto Literal2Cpp::bytesLiteral2CppBytes(const std::string &bytes_literal) -> std::vector<uint8_t>
{
    static constexpr int BASE = 16;
    auto hex_part = std::string_view(bytes_literal).substr(0, bytes_literal.size() - 1);
    if (hex_part.size() % 2 != 0)
    {
        throw std::invalid_argument(
            "Invalid bytes literal: number of hex digits must be even for proper byte conversion");
    }
    std::vector<uint8_t> result{};
    result.reserve(hex_part.size() / 2);
    for (size_t i = 0; i < hex_part.size(); i += 2)
    {
        auto sub_string = hex_part.substr(i, 2);
        uint8_t byte_val = 0;
        auto [ptr, ec] = std::from_chars(sub_string.begin(), sub_string.end(), byte_val, BASE);
        result.push_back(byte_val);
    }
    return result;
}

} // namespace sonnx
