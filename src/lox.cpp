#include "lox.hpp"

#include "ast_printer.hpp"
#include "parser.hpp"
#include "scanner.hpp"

#include <iostream>
#include <string>

bool Lox::had_error = false;

void Lox::error(Token token, std::string_view message) {
  if (token.get_type() == Token::Type::EOF) {
    report(token.get_line(), " at end", message);
  } else {
    // convert token lexeme to std::string to build the message
    report(token.get_line(), " at '" + std::string{token.get_lexeme()} + "'",
           message);
  }
}

int Lox::main(int /*argc*/, char *argv[]) {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  const std::string command{argv[1]};
  Scanner scanner{std::string(argv[2])};

  auto tokens = scanner.scan_tokens();

  // for (auto &token : tokens) {
  //   std::cout << token.to_string() << '\n';
  // }

  if (command == "tokenize") {
    // for (auto &token : tokens) {
    //   std::cout << token.to_string() << '\n';
    // }
    if (had_error) {
      return 65; // Lox convention: syntax‑error exit code
    }
  } else if (command == "parse") {
    Parser parser{tokens};
    Expression &expression = parser.parse();

    if (had_error) {
      return 65; // Lox convention: syntax‑error exit code
    }

    // ASTPrinter printer{};
    // printer.print(*(expression.value()));
  } else {
    return -1;
  }

  // std::cout << expression.value().get().

  return 0;
}
