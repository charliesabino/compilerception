#pragma once
#include "token.hpp"
#include <memory>
#include <string>

class ExpressionVisitor;

struct Expression {
  virtual ~Expression() = default;
  virtual auto accept(const ExpressionVisitor &visitor) const
      -> std::string = 0;
};

struct BinaryExpression : public Expression {
  std::shared_ptr<Expression> left;
  Token op;
  std::shared_ptr<Expression> right;

  BinaryExpression(std::shared_ptr<Expression> left, Token op,
                   std::shared_ptr<Expression> right)
      : left(std::move(left)), op(op), right(std::move(right)) {}

  auto accept(const ExpressionVisitor &visitor) const -> std::string override;
};

struct GroupingExpression : public Expression {
  std::shared_ptr<Expression> expression;

  explicit GroupingExpression(std::shared_ptr<Expression> expression)
      : expression(std::move(expression)) {}
  auto accept(const ExpressionVisitor &visitor) const -> std::string override;
};

struct LiteralExpression : public Expression {
  TokenLiteral literal;

  explicit LiteralExpression(TokenLiteral literal) : literal(literal) {}

  auto accept(const ExpressionVisitor &visitor) const -> std::string override;
};

struct UnaryExpression : public Expression {
  Token op;
  std::shared_ptr<Expression> right;

  UnaryExpression(Token op, std::shared_ptr<Expression> right)
      : op(op), right(std::move(right)) {}

  auto accept(const ExpressionVisitor &visitor) const -> std::string override;
};

class ExpressionVisitor {
public:
  virtual ~ExpressionVisitor() = default;
  virtual auto visit(const BinaryExpression &expr) const -> std::string = 0;
  virtual auto visit(const GroupingExpression &expr) const -> std::string = 0;
  virtual auto visit(const LiteralExpression &expr) const -> std::string = 0;
  virtual auto visit(const UnaryExpression &expr) const -> std::string = 0;
};
