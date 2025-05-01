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

struct ParseError {
  std::string_view message;

  explicit ParseError(std::string_view message) : message{message} {}
};

class Parser {
public:
  explicit Parser(std::vector<Token> tokens) : tokens{std::move(tokens)} {}

  auto parse() -> std::expected<std::shared_ptr<Expression>, ParseError> {
    return expression();
  }

private:
  std::vector<Token> tokens;
  int current{};

  static auto error(Token token, std::string_view message) {
    Lox::error(token, message);
    return ParseError{message};
  }

  auto peek() { return tokens.at(current); }
  auto previous() { return tokens.at(current - 1); }
  auto advance() {
    if (!is_at_end()) {
      current++;
    }
    return previous();
  }

  auto is_at_end() -> bool { return peek().get_type() == Token::Type::EOF; }
  auto check(Token::Type type) {
    return (!is_at_end() && peek().get_type() == type);
  }

  template <typename... Args> auto match(Args... types) {
    return (... || (check(types) ? (advance(), true) : false));
  }

  auto consume(Token::Type type, std::string_view message)
      -> std::expected<Token, ParseError> {
    if (check(type)) {
      return advance();
    }

    return std::unexpected(error(peek(), message));
  }

  auto primary() -> std::expected<std::shared_ptr<Expression>, ParseError> {
    if (match(Token::Type::FALSE)) {
      return std::make_shared<LiteralExpression>(false);
    }
    if (match(Token::Type::TRUE)) {
      return std::make_shared<LiteralExpression>(true);
    }
    if (match(Token::Type::NIL)) {
      return std::make_shared<LiteralExpression>(std::monostate{});
    }

    if (match(Token::Type::NUMBER, Token::Type::STRING)) {
      return std::make_shared<LiteralExpression>(previous().get_literal());
    }

    if (match(Token::Type::LEFT_PAREN)) {
      auto expr = expression();
      if (!expr.has_value()) {
        return expr;
      }
      const auto res =
          consume(Token::Type::RIGHT_PAREN, "Expected ')' after expression.");

      if (res.has_value()) {
        return std::make_shared<GroupingExpression>(expr.value());
      }

      return std::unexpected{res.error()};
    }
    return std::unexpected(error(peek(), "Expect expression."));
  }

  auto unary() -> std::expected<std::shared_ptr<Expression>, ParseError> {
    if (match(Token::Type::BANG, Token::Type::MINUS)) {
      auto op = previous();
      auto right = unary();
      if (!right.has_value()) {
        return right;
      }
      return std::make_shared<UnaryExpression>(op, right.value());
    }
    return primary();
  }

  auto factor() -> std::expected<std::shared_ptr<Expression>, ParseError> {
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
      expr =
          std::make_shared<BinaryExpression>(expr.value(), opr, right.value());
    }

    return expr;
  }
  auto term() -> std::expected<std::shared_ptr<Expression>, ParseError> {
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
      expr =
          std::make_shared<BinaryExpression>(expr.value(), opr, right.value());
    }

    return expr;
  }
  auto comparison() -> std::expected<std::shared_ptr<Expression>, ParseError> {
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
      expr =
          std::make_shared<BinaryExpression>(expr.value(), opr, right.value());
    }

    return expr;
  }
  auto equality() -> std::expected<std::shared_ptr<Expression>, ParseError> {
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
      expr =
          std::make_shared<BinaryExpression>(expr.value(), opr, right.value());
    }

    return expr;
  }
  auto expression() -> std::expected<std::shared_ptr<Expression>, ParseError> {
    return equality();
  }

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
