#ifndef SCANNER_H
#define SCANNER_H

#include "token.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Scanner {
private:
  std::string_view filename;
  std::string source;
  std::vector<Token> tokens;
  uint32_t start{0};
  uint32_t current{0};
  uint32_t line{1};
  bool has_error{false};

  void tokenize();

  [[nodiscard]]
  auto is_at_end() const -> bool;

  auto advance() -> char;
  [[nodiscard]]
  auto match(char expected) -> bool;
  [[nodiscard]]
  auto peek() -> char;
  auto peek_next() -> char;
  auto add_token(Token::Type type) -> void;
  auto add_token(Token::Type type, TokenLiteral literal) -> void;
  auto scan_token() -> void;
  auto string() -> void;
  auto number() -> void;
  auto identifier() -> void;

public:
  Scanner(const std::string &source) : source(source) {}

  auto scan_tokens() -> std::vector<Token> &;

  [[nodiscard]]
  auto errored() const -> bool {
    return has_error;
  }
};

static const std::unordered_map<std::string_view, Token::Type> keyword_to_token{
    {"and", Token::Type::AND},       {"class", Token::Type::CLASS},
    {"else", Token::Type::ELSE},     {"false", Token::Type::FALSE},
    {"for", Token::Type::FOR},       {"fun", Token::Type::FUN},
    {"if", Token::Type::IF},         {"nil", Token::Type::NIL},
    {"or", Token::Type::OR},         {"print", Token::Type::PRINT},
    {"return", Token::Type::RETURN}, {"super", Token::Type::SUPER},
    {"this", Token::Type::THIS},     {"true", Token::Type::TRUE},
    {"var", Token::Type::VAR},       {"while", Token::Type::WHILE}};

#endif // !SCANNER_H
