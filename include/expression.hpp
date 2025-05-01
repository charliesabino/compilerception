#pragma once
#include "token.hpp"
#include <memory>
#include <string>

using Expression = std::variant<BinaryExpression, GroupingExpression, LiteralExpression, UnaryExpression>;

class ExpressionVisitor;

struct BinaryExpression {
  Expression left;
  Token op;
  Expression right;

  BinaryExpression(std::shared_ptr<Expression> left, Token op,
                   std::shared_ptr<Expression> right)
      : left(std::move(left)), op(op), right(std::move(right)) {}

  auto accept(const ExpressionVisitor &visitor) const -> std::string ;
};

struct GroupingExpression : public Expression {
  std::shared_ptr<Expression> expression;

  explicit GroupingExpression(std::shared_ptr<Expression> expression)
      : expression(std::move(expression)) {}
  auto accept(const ExpressionVisitor &visitor) const -> std::string;
};

struct LiteralExpression : public Expression {
  TokenLiteral literal;

  explicit LiteralExpression(TokenLiteral literal) : literal(literal) {}

  auto accept(const ExpressionVisitor &visitor) const -> std::string;
};

struct UnaryExpression {
  Token op;
  Expression right;

  UnaryExpression(Token op, Expression right) : op(op), right(right) {}

  auto accept(const ExpressionVisitor &visitor) const -> std::string;
};