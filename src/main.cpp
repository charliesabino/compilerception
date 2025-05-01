#include "lox.hpp"
#include <iostream>

constexpr auto result = Lox::interpret("1 + 2 * 3");

int main() {
  std::cout << result << '\n';
  return 0;
}
