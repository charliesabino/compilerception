#include "lox.hpp"
#include "program.hpp"

#include "parser.hpp"
#include "scanner.hpp"

#include <iostream>
#include <string>

bool Lox::had_error = false;

void Lox::error(Token token, std::string_view message) {
  had_error = true;
}

int Lox::main(int /*argc*/, char *argv[]) {
  const std::string command{argv[1]};

  // Use the embedded program string
  Scanner scanner{std::string(lox::program)};
  auto tokens = scanner.scan_tokens();

  if (command == "tokenize") {
    if (had_error) {
      return 65;
    }
  } else if (command == "parse") {
    Parser parser{tokens};
    auto expression = parser.parse();

    if (had_error) {
      return 65;
    }
  } else {
    return -1;
  }

  return 0;
}
