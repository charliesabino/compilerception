#include "ast_printer.hpp"
#include "token.hpp"
#include <format>
#include <iostream>
#include <string_view>

template <typename... Args>
auto ASTPrinter::parenthesize(std::string_view name, const Args &...exprs) const
    -> std::string {
  std::string result = "(" + std::string(name);
  ((result += " " + exprs->accept(*this)), ...);
  result += ")";
  return result;
}

auto ASTPrinter::print(const Expression &expr) -> void {
  std::cout << expr.accept(*this) << '\n';
}

auto ASTPrinter::visit(const BinaryExpression &expr) const -> std::string {
  return parenthesize(expr.op.get_lexeme(), expr.left, expr.right);
}

auto ASTPrinter::visit(const GroupingExpression &expr) const -> std::string {
  return parenthesize("group", expr.expression);
}

auto ASTPrinter::visit(const LiteralExpression &expr) const -> std::string {
  return token_literal_to_string(expr.literal);
}

auto ASTPrinter::visit(const UnaryExpression &expr) const -> std::string {
  return parenthesize(expr.op.get_lexeme(), expr.right);
}
