#include "scanner.hpp"
#include "lox.hpp"
#include "token.hpp"
#include <cctype>
#include <format>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>

using std::isalpha;

Scanner::Scanner(std::string &&filename) : filename(std::move(filename)) {
  std::ifstream file{filename};
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  source = buffer.str();
}

[[nodiscard]] auto Scanner::is_at_end() const -> bool {
  return current >= source.size();
}

auto Scanner::advance() -> char { return source[current++]; }

auto Scanner::match(char expected) -> bool {
  if (is_at_end() || source[current] != expected) {
    return false;
  }
  current++;
  return true;
}

auto Scanner::peek() -> char {
  if (is_at_end()) {
    return '\0';
  }
  return source[current];
}

auto Scanner::peek_next() -> char {
  if (current + 1 >= source.size()) {
    return '\0';
  }
  return source[current + 1];
}

auto Scanner::add_token(const Token::Type type) -> void {
  tokens.emplace_back(
      type, std::string_view(source.data() + start, current - start), line);
}

auto Scanner::add_token(const Token::Type type, const TokenLiteral literal)
    -> void {
  tokens.emplace_back(type,
                      std::string_view(source.data() + start, current - start),
                      literal, line);
}

auto Scanner::string() -> void {
  advance();
  while (peek() != '"' && !is_at_end()) {
    if (peek() == '\n') {
      line++;
    }
    advance();
  }
  if (is_at_end()) {
    Lox::error(line, "Unterminated string.");
    return;
  }
  advance(); // closing "

  auto size = current - start - 2;

  auto str = std::string_view(source.data() + start + 1, size);

  add_token(Token::Type::STRING, str);
}

auto is_digit(auto chr) -> bool { return chr >= '0' && chr <= '9'; }

auto Scanner::number() -> void {
  while (is_digit(peek())) {
    advance();
  }
  if (peek() == '.' && is_digit(peek_next())) {
    advance();

    while (is_digit(peek())) {
      advance();
    }
  }

  auto str = std::string_view{source};
  str = str.substr(start, current - start);

  try {
    auto value = std::stod(std::string(str));
    add_token(Token::Type::NUMBER, value);
  } catch (const std::exception &e) {
    Lox::error(line, std::format("Invalid number format: {}", str));
  }
}

auto Scanner::identifier() -> void {
  while ((isalnum(peek()) != 0) || peek() == '_') {
    advance();
  }

  auto str = std::string_view{source};
  str = str.substr(start, current - start);

  // could use exceptions
  // but non keywords are probably more common than keywords, and exceptions are
  // expensive would have to measure
  auto type = keyword_to_token.contains(str) ? keyword_to_token.at(str)
                                             : Token::Type::IDENTIFIER;
  add_token(type);
}

auto Scanner::scan_token() -> void {
  char chr = advance();

  switch (chr) {
  case '(':
    add_token(Token::Type::LEFT_PAREN);
    break;
  case ')':
    add_token(Token::Type::RIGHT_PAREN);
    break;
  case '{':
    add_token(Token::Type::LEFT_BRACE);
    break;
  case '}':
    add_token(Token::Type::RIGHT_BRACE);
    break;
  case ',':
    add_token(Token::Type::COMMA);
    break;
  case '.':
    add_token(Token::Type::DOT);
    break;
  case '-':
    add_token(Token::Type::MINUS);
    break;
  case '+':
    add_token(Token::Type::PLUS);
    break;
  case ';':
    add_token(Token::Type::SEMICOLON);
    break;
  case '*':
    add_token(Token::Type::STAR);
    break;
  case '!':
    add_token(match('=') ? Token::Type::BANG_EQUAL : Token::Type::BANG);
    break;
  case '=':
    add_token(match('=') ? Token::Type::EQUAL_EQUAL : Token::Type::EQUAL);
    break;
  case '<':
    add_token(match('=') ? Token::Type::LESS_EQUAL : Token::Type::LESS);
    break;
  case '>':
    add_token(match('=') ? Token::Type::GREATER_EQUAL : Token::Type::GREATER);
    break;
  case '"':
    string();
    break;
  case '/':
    if (match('/')) {
      while (peek() != '\n' && !is_at_end()) {
        advance();
      }
      break;
    }
    add_token(Token::Type::SLASH);
  case ' ':
  case '\r':
  case '\t':
    break;
  case '\n':
    line++;
    break;
  default:
    if (is_digit(chr)) {
      number();
    } else if ((isalpha(chr) != 0) || chr == '_') {
      identifier();
    } else {
      Lox::error(line, std::format("Unexpected character: {}", chr));
    }
    break;
  }
}

auto Scanner::scan_tokens() -> std::vector<Token> & {
  while (!is_at_end() && !has_error) {
    start = current;
    scan_token();
  }
  start = current;
  add_token(Token::Type::EOF);
  return tokens;
}
