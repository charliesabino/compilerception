#pragma once

#include "expression.hpp"
#include "lox.hpp"
#include "token.hpp"
#include <expected>
#include <memory>
#include <set>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

std::vector<BinaryExpression> binary_expressions;
std::vector<GroupingExpression> grouping_expressions;
std::vector<LiteralExpression> literal_expressions;
std::vector<UnaryExpression> unary_expressions;

size_t expression_index = 0;

struct ParseError {
  std::string_view message;

  explicit ParseError(std::string_view message) : message{message} {}
};

class Parser {
public:
  explicit Parser(std::vector<Token> tokens) : tokens{std::move(tokens)} {}

  auto parse() -> Expression & { return expression(); }

private:
  std::vector<Token> tokens;
  int current{};

  static auto error(Token token, std::string_view message) {
    Lox::error(token, message);
    return ParseError{message};
  }

  constexpr auto peek() { return tokens.at(current); }
  constexpr auto previous() { return tokens.at(current - 1); }
  constexpr auto advance() {
    if (!is_at_end()) {
      current++;
    }
    return previous();
  }

  constexpr auto is_at_end() -> bool { return peek().get_type() == Token::Type::EOF; }
  constexpr auto check(Token::Type type) {
    return (!is_at_end() && peek().get_type() == type);
  }

  template <typename... Args> constexpr auto match(Args... types) {
    return (... || (check(types) ? (advance(), true) : false));
  }

  constexpr auto consume(Token::Type type, std::string_view message) -> Token {
    return advance();
  }

  constexpr auto primary() -> Expression & {
    if (match(Token::Type::FALSE)) {
      auto &literal_expression =  literal_expressions.emplace_back(false);
      return literal_expression;
    }
    if (match(Token::Type::TRUE)) {
      auto &literal_expression = literal_expressions.emplace_back(true);
      return literal_expression;
    }
    if (match(Token::Type::NIL)) {
      auto &literal_expression =
          literal_expressions.emplace_back(std::monostate{});
      return literal_expression;
    }

    if (match(Token::Type::NUMBER, Token::Type::STRING)) {
      auto &literal_expression =
          literal_expressions.emplace_back(previous().get_literal());
      return literal_expression;
    }

    if (match(Token::Type::LEFT_PAREN)) {
      Expression &expr = expression();
      consume(Token::Type::RIGHT_PAREN, "Expected ')' after expression.");

      auto &grouping_expression =
          grouping_expressions.emplace_back(GroupingExpression{expr});
      return grouping_expression;
    }

    throw error(peek(), "Expected expression.");
  }

  constexpr auto unary() -> Expression & {
    if (match(Token::Type::BANG, Token::Type::MINUS)) {
      auto op = previous();
      Expression &right = unary();
      auto &unary_expression = unary_expressions.emplace_back(UnaryExpression{op, right});
      return unary_expression;
    }
    return primary();
  }

  constexpr auto factor() -> Expression & {
    Expression &expr = unary();
    while (match(Token::Type::SLASH, Token::Type::STAR)) {
      auto opr = previous();
      Expression &right = unary();
      auto &binary_expression =
          binary_expressions.emplace_back(BinaryExpression{expr, opr, right});
      expr = binary_expressions[expression_index++];
    }

    return expr;
  }
  constexpr auto term() -> Expression & {
    Expression &expr = factor();
    while (match(Token::Type::PLUS, Token::Type::MINUS)) {
      auto opr = previous();
      Expression &right = factor();
      auto &binary_expression =
          binary_expressions.emplace_back(BinaryExpression{expr, opr, right});
      expr = binary_expressions[expression_index++];
    }

    return expr;
  }
  constexpr auto comparison() -> Expression & {
    Expression &expr = term();
    while (match(Token::Type::GREATER, Token::Type::GREATER_EQUAL,
                 Token::Type::LESS, Token::Type::LESS_EQUAL)) {
      auto opr = previous();
      Expression &right = term();
      auto &binary_expression =
          binary_expressions.emplace_back(BinaryExpression{expr, opr, right});
      expr = binary_expressions[expression_index++];
    }

    return expr;
  }
  constexpr auto equality() -> Expression & {
    Expression &expr = comparison();
    while (match(Token::Type::BANG_EQUAL, Token::Type::EQUAL_EQUAL)) {
      auto opr = previous();
      Expression &right = comparison();
      auto &binary_expression =
          binary_expressions.emplace_back(BinaryExpression{expr, opr, right});
      expr = binary_expressions[expression_index++];
    }

    return expr;
  }
  constexpr auto expression() -> Expression & { return equality(); }

  auto synchronize() {
    static auto sync_points = std::set<Token::Type>{
        Token::Type::CLASS, Token::Type::FUN,   Token::Type::VAR,
        Token::Type::FOR,   Token::Type::IF,    Token::Type::WHILE,
        Token::Type::PRINT, Token::Type::RETURN};

    advance();

    while (!is_at_end()) {
      if (previous().get_type() == Token::Type::SEMICOLON) {
        return;
      }
      if (sync_points.contains(peek().get_type())) {
        return;
      }
      advance();
    }
  }
};
