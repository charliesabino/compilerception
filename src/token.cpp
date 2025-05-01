#include "token.hpp"
#include <cmath>
#include <format>
#include <string>
#include <variant>

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};

auto token_literal_to_string(TokenLiteral literal) -> std::string {
  return std::visit(
      overloaded{[](std::monostate) { return std::string{"nil"}; },
                 [](double arg) {
                   if (std::floor(arg) == arg) {
                     return std::format("{:.1f}", arg);
                   }
                   return std::format("{}", arg);
                 },
                 [](auto arg) { return std::format("{}", arg); }},
      literal);
};

auto Token::to_string() const -> std::string {
  auto lit =
      literal.has_value() ? token_literal_to_string(literal.value()) : "null";
  return std::format("{} {} {}", type_to_str[static_cast<uint8_t>(type)],
                     lexeme, lit);
}
