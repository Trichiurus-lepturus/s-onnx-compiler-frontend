#ifndef LITERAL_2_CPP_HPP
#define LITERAL_2_CPP_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace sonnx
{

class Literal2Cpp
{
  public:
    static auto integerLiteral2CppInteger(const std::string &integer_literal) noexcept(false)
        -> std::variant<uint32_t, uint64_t>;
    static auto stringLiteral2CppString(const std::string &string_literal) noexcept(true) -> std::string;
    static auto bytesLiteral2CppBytes(const std::string &bytes_literal) noexcept(false) -> std::vector<uint8_t>;
};

} // namespace sonnx

#endif // LITERAL_2_CPP_HPP
