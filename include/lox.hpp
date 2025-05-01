#pragma once

#include <concepts>
#include <format>
#include <iostream>
#include <string_view>

#include "token.hpp"

class Scanner;
class Parser;
class ASTPrinter;

class Lox {
public:
  static bool had_error;

  static void report(std::integral auto line, std::string_view where,
                     std::string_view message) {
    std::cerr << std::format("[line {}] Error{}: {}\n", line, where, message);
    had_error = true;
  }

  static void error(Token token, std::string_view message);

  template <std::integral T>
  static void error(T line, std::string_view message) {
    report(line, "", message);
  }

  static auto main(int, char *argv[]) -> int;
};
