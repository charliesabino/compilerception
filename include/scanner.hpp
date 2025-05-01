#pragma once

#include "token.hpp"
#include <array>
#include <string_view>
#include <vector>

class Scanner {
public:
  constexpr explicit Scanner(std::string_view source) : source(source) {}

  constexpr auto scan_tokens() -> std::vector<Token> {
    while (!is_at_end()) {
      start = current;
      scan_token();
    }
    add_token(Token::Type::EOF);
    return tokens;
  }

private:
  std::string_view source;
  std::vector<Token> tokens;
  size_t start{0};
  size_t current{0};
  size_t line{1};

  constexpr auto is_at_end() const -> bool {
    return current >= source.size();
  }

  constexpr auto advance() -> char {
    return source[current++];
  }

  constexpr auto match(char expected) -> bool {
    if (is_at_end() || source[current] != expected) {
      return false;
    }
    current++;
    return true;
  }

  constexpr auto peek() const -> char {
    if (is_at_end()) {
      return '\0';
    }
    return source[current];
  }

  constexpr auto peek_next() const -> char {
    if (current + 1 >= source.size()) {
      return '\0';
    }
    return source[current + 1];
  }

  constexpr auto add_token(Token::Type type) -> void {
    tokens.emplace_back(type, source.substr(start, current - start), line);
  }

  constexpr auto add_token(Token::Type type, TokenLiteral literal) -> void {
    tokens.emplace_back(type, source.substr(start, current - start), literal, line);
  }

  constexpr auto is_digit(char c) -> bool {
    return c >= '0' && c <= '9';
  }

  constexpr auto is_alpha(char c) -> bool {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  constexpr auto is_alphanumeric(char c) -> bool {
    return is_alpha(c) || is_digit(c);
  }

  constexpr auto number() -> void {
    while (is_digit(peek())) {
      advance();
    }
    if (peek() == '.' && is_digit(peek_next())) {
      advance();
      while (is_digit(peek())) {
        advance();
      }
    }
    add_token(Token::Type::NUMBER, 0.0); // Simplified for compile-time
  }

  constexpr auto identifier() -> void {
    while (is_alphanumeric(peek())) {
      advance();
    }
    auto text = source.substr(start, current - start);
    auto type = Token::Type::IDENTIFIER;
    for (const auto& [keyword, token_type] : keyword_to_token) {
      if (text == keyword) {
        type = token_type;
        break;
      }
    }
    add_token(type);
  }

  constexpr auto string() -> void {
    while (peek() != '"' && !is_at_end()) {
      if (peek() == '\n') {
        line++;
      }
      advance();
    }
    if (is_at_end()) {
      return;
    }
    advance();
    auto value = source.substr(start + 1, current - start - 2);
    add_token(Token::Type::STRING, value);
  }

  constexpr auto scan_token() -> void {
    char c = advance();
    switch (c) {
      case '(': add_token(Token::Type::LEFT_PAREN); break;
      case ')': add_token(Token::Type::RIGHT_PAREN); break;
      case '{': add_token(Token::Type::LEFT_BRACE); break;
      case '}': add_token(Token::Type::RIGHT_BRACE); break;
      case ',': add_token(Token::Type::COMMA); break;
      case '.': add_token(Token::Type::DOT); break;
      case '-': add_token(Token::Type::MINUS); break;
      case '+': add_token(Token::Type::PLUS); break;
      case ';': add_token(Token::Type::SEMICOLON); break;
      case '*': add_token(Token::Type::STAR); break;
      case '!': add_token(match('=') ? Token::Type::BANG_EQUAL : Token::Type::BANG); break;
      case '=': add_token(match('=') ? Token::Type::EQUAL_EQUAL : Token::Type::EQUAL); break;
      case '<': add_token(match('=') ? Token::Type::LESS_EQUAL : Token::Type::LESS); break;
      case '>': add_token(match('=') ? Token::Type::GREATER_EQUAL : Token::Type::GREATER); break;
      case '/':
        if (match('/')) {
          while (peek() != '\n' && !is_at_end()) {
            advance();
          }
        } else {
          add_token(Token::Type::SLASH);
        }
        break;
      case ' ':
      case '\r':
      case '\t':
        break;
      case '\n':
        line++;
        break;
      case '"': string(); break;
      default:
        if (is_digit(c)) {
          number();
        } else if (is_alpha(c)) {
          identifier();
        }
        break;
    }
  }

  static constexpr std::array<std::pair<std::string_view, Token::Type>, 16> keyword_to_token{{
    {"and", Token::Type::AND},
    {"class", Token::Type::CLASS},
    {"else", Token::Type::ELSE},
    {"false", Token::Type::FALSE},
    {"for", Token::Type::FOR},
    {"fun", Token::Type::FUN},
    {"if", Token::Type::IF},
    {"nil", Token::Type::NIL},
    {"or", Token::Type::OR},
    {"print", Token::Type::PRINT},
    {"return", Token::Type::RETURN},
    {"super", Token::Type::SUPER},
    {"this", Token::Type::THIS},
    {"true", Token::Type::TRUE},
    {"var", Token::Type::VAR},
    {"while", Token::Type::WHILE}
  }};
};
