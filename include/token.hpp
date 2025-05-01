#include <optional>
#include <string_view>
#include <variant>
#pragma once

#include <array>
#include <cstdint>
#include <string>

#undef EOF

using TokenLiteral =
    std::variant<std::monostate, std::string_view, double, bool>;

class Token {
public:
  enum class Type : uint8_t {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    WHILE,
    VAR,
    EOF,
    NUM_TYPES,
  };

  Token(Type type, std::string_view lexeme, TokenLiteral literal, uint32_t line)
      : type{type}, lexeme{lexeme}, literal{literal}, line{line} {}

  Token(Type type, std::string_view lexeme, uint32_t line)
      : type{type}, lexeme{lexeme}, line{line} {}

  [[nodiscard]] auto to_string() const -> std::string;
  [[nodiscard]] auto get_lexeme() const -> std::string_view { return lexeme; }
  [[nodiscard]] auto get_type() const -> Type { return type; }
  [[nodiscard]] auto get_literal() const -> TokenLiteral {
    return literal.value();
  }
  [[nodiscard]] auto get_line() const -> uint32_t { return line; }

private:
  std::string_view lexeme;
  Type type;
  std::optional<TokenLiteral> literal;
  uint32_t line;
};