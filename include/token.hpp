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
    // Single-character tokens. LEFT_PAREN,
    LEFT_PAREN = 0,
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

  constexpr Token(Type type, std::string_view lexeme, TokenLiteral literal, uint32_t line)
      : type{type}, lexeme{lexeme}, literal{literal}, line{line} {}

  constexpr Token(Type type, std::string_view lexeme, uint32_t line)
      : type{type}, lexeme{lexeme}, line{line} {}

  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    return lexeme;
  }
  
  [[nodiscard]] constexpr auto get_lexeme() const -> std::string_view { 
    return lexeme; 
  }
  
  [[nodiscard]] constexpr auto get_type() const -> Type { 
    return type; 
  }
  
  [[nodiscard]] constexpr auto get_literal() const -> TokenLiteral {
    return literal.value();
  }
  
  [[nodiscard]] constexpr auto get_line() const -> uint32_t { 
    return line; 
  }

private:
  std::string_view lexeme;
  Type type;
  std::optional<TokenLiteral> literal;
  uint32_t line;
};

constexpr auto token_literal_to_string(TokenLiteral literal) -> std::string_view {
  return std::visit(
    [](auto&& arg) -> std::string_view {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, std::monostate>) {
        return "nil";
      } else if constexpr (std::is_same_v<T, bool>) {
        return arg ? "true" : "false";
      } else if constexpr (std::is_same_v<T, double>) {
        return "number"; // Can't convert to string at compile time
      } else if constexpr (std::is_same_v<T, std::string_view>) {
        return arg;
      } else {
        return "unknown";
      }
    },
    literal
  );
}

static constexpr std::array type_to_str{
    "LEFT_PAREN", "RIGHT_PAREN",   "LEFT_BRACE", "RIGHT_BRACE", "COMMA",
    "DOT",        "MINUS",         "PLUS",       "SEMICOLON",   "SLASH",
    "STAR",       "BANG",          "BANG_EQUAL", "EQUAL",       "EQUAL_EQUAL",
    "GREATER",    "GREATER_EQUAL", "LESS",       "LESS_EQUAL",  "IDENTIFIER",
    "STRING",     "NUMBER",        "AND",        "CLASS",       "ELSE",
    "FALSE",      "FUN",           "FOR",        "IF",          "NIL",
    "OR",         "PRINT",         "RETURN",     "SUPER",       "THIS",
    "TRUE",       "WHILE",         "VAR",        "EOF",
};
