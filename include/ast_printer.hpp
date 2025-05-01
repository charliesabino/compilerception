#pragma once
#include "expression.hpp"

class ASTPrinter : public ExpressionVisitor {
public:
  auto print(const Expression &expr) -> void;

  auto visit(const BinaryExpression &expr) const -> std::string override;
  auto visit(const GroupingExpression &expr) const -> std::string override;
  auto visit(const LiteralExpression &expr) const -> std::string override;
  auto visit(const UnaryExpression &expr) const -> std::string override;

private:
  template <typename... Args>
  std::string parenthesize(std::string_view name, const Args &...exprs) const;
};
