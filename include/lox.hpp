#pragma once

#include "expression.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include <string_view>

class Lox {
public:
  constexpr static auto interpret(std::string_view source) -> std::string_view {
    Scanner scanner{source};
    auto tokens = scanner.scan_tokens();
    Parser parser{std::move(tokens)};
    auto expression = parser.parse();
    if (!expression.has_value()) {
      return "Parse error";
    }
    return evaluate_expression(expression.value());
  }
};
