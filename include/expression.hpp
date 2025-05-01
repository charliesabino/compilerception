#pragma once
#include "token.hpp"
#include <string>

class ExpressionVisitor;

class Expression {
public:
  // virtual ~Expression() = default;
  // virtual auto accept(const ExpressionVisitor &visitor) const
  //     -> std::string = 0;
};

class BinaryExpression;
class GroupingExpression;
class LiteralExpression;
class UnaryExpression;

class BinaryExpression : public Expression {
  Expression &left;
  Token op;
  Expression &right;

public:
  BinaryExpression(Expression &left, Token op, Expression &right)
      : left(left), op(op), right(right) {}

  // auto accept(const ExpressionVisitor &visitor) const -> std::string;
};

class GroupingExpression : public Expression {
  Expression &expression;

public:
  explicit GroupingExpression(Expression &expression)
      : expression(expression) {}

  // auto accept(const ExpressionVisitor &visitor) const -> std::string;
};

class LiteralExpression : public Expression {
  TokenLiteral literal;

public:
  explicit LiteralExpression(TokenLiteral literal) : literal(literal) {}

  // auto accept(const ExpressionVisitor &visitor) const -> std::string;
};

class UnaryExpression : public Expression {
  Token op;
  Expression &right;

public:
  UnaryExpression(Token op, Expression &right) : op(op), right(right) {}

  // auto accept(const ExpressionVisitor &visitor) const -> std::string;
};

class ExpressionVisitor {};
