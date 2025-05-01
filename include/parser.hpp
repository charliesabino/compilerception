#pragma once

#include "expression.hpp"
#include "token.hpp"
#include <array>
#include <expected>
#include <string_view>
#include <vector>

struct ParseError {
  std::string_view message;
  constexpr explicit ParseError(std::string_view message) : message{message} {}
};

class Parser {
public:
  constexpr explicit Parser(std::vector<Token> tokens) : tokens{std::move(tokens)} {}

  constexpr auto parse() -> std::expected<AnyExpression, ParseError> {
    return expression();
  }

private:
  std::vector<Token> tokens;
  size_t current{};

  constexpr auto peek() const -> Token {
    return tokens.at(current);
  }

  constexpr auto previous() const -> Token {
    return tokens.at(current - 1);
  }

  constexpr auto advance() -> Token {
    if (!is_at_end()) {
      current++;
    }
    return previous();
  }

  constexpr auto is_at_end() const -> bool {
    return peek().get_type() == Token::Type::EOF;
  }

  constexpr auto check(Token::Type type) const -> bool {
    return !is_at_end() && peek().get_type() == type;
  }

  template <typename... Args>
  constexpr auto match(Args... types) -> bool {
    return (... || (check(types) ? (advance(), true) : false));
  }

  constexpr auto consume(Token::Type type, std::string_view message)
      -> std::expected<Token, ParseError> {
    if (check(type)) {
      return advance();
    }
    return std::unexpected(ParseError{message});
  }

  constexpr auto primary() -> std::expected<AnyExpression, ParseError> {
    if (match(Token::Type::FALSE)) {
      return make_expression<LiteralExpression>(false);
    }
    if (match(Token::Type::TRUE)) {
      return make_expression<LiteralExpression>(true);
    }
    if (match(Token::Type::NIL)) {
      return make_expression<LiteralExpression>(std::monostate{});
    }

    if (match(Token::Type::NUMBER, Token::Type::STRING)) {
      return make_expression<LiteralExpression>(previous().get_literal());
    }

    if (match(Token::Type::LEFT_PAREN)) {
      auto expr = expression();
      if (!expr.has_value()) {
        return expr;
      }
      const auto res = consume(Token::Type::RIGHT_PAREN, "Expected ')' after expression.");
      if (res.has_value()) {
        return make_expression<GroupingExpression>(expr.value());
      }
      return std::unexpected{res.error()};
    }
    return std::unexpected(ParseError{"Expect expression."});
  }

  constexpr auto unary() -> std::expected<AnyExpression, ParseError> {
    if (match(Token::Type::BANG, Token::Type::MINUS)) {
      auto op = previous();
      auto right = unary();
      if (!right.has_value()) {
        return right;
      }
      return make_expression<UnaryExpression>(op, right.value());
    }
    return primary();
  }

  constexpr auto factor() -> std::expected<AnyExpression, ParseError> {
    auto expr = unary();
    if (!expr.has_value()) {
      return expr;
    }
    while (match(Token::Type::SLASH, Token::Type::STAR)) {
      auto opr = previous();
      auto right = unary();
      if (!right.has_value()) {
        return right;
      }
      expr = make_expression<BinaryExpression>(expr.value(), opr, right.value());
    }
    return expr;
  }

  constexpr auto term() -> std::expected<AnyExpression, ParseError> {
    auto expr = factor();
    if (!expr.has_value()) {
      return expr;
    }
    while (match(Token::Type::PLUS, Token::Type::MINUS)) {
      auto opr = previous();
      auto right = factor();
      if (!right.has_value()) {
        return right;
      }
      expr = make_expression<BinaryExpression>(expr.value(), opr, right.value());
    }
    return expr;
  }

  constexpr auto comparison() -> std::expected<AnyExpression, ParseError> {
    auto expr = term();
    if (!expr.has_value()) {
      return expr;
    }
    while (match(Token::Type::GREATER, Token::Type::GREATER_EQUAL,
                 Token::Type::LESS, Token::Type::LESS_EQUAL)) {
      auto opr = previous();
      auto right = term();
      if (!right.has_value()) {
        return right;
      }
      expr = make_expression<BinaryExpression>(expr.value(), opr, right.value());
    }
    return expr;
  }

  constexpr auto equality() -> std::expected<AnyExpression, ParseError> {
    auto expr = comparison();
    if (!expr.has_value()) {
      return expr;
    }
    while (match(Token::Type::BANG_EQUAL, Token::Type::EQUAL_EQUAL)) {
      auto opr = previous();
      auto right = comparison();
      if (!right.has_value()) {
        return right;
      }
      expr = make_expression<BinaryExpression>(expr.value(), opr, right.value());
    }
    return expr;
  }

  constexpr auto expression() -> std::expected<AnyExpression, ParseError> {
    return equality();
  }
};
