#pragma once
#include "token.hpp"
#include <string_view>
#include <variant>

// Forward declarations
struct BinaryExpression;
struct GroupingExpression;
struct LiteralExpression;
struct UnaryExpression;

using AnyExpression = std::variant<
  BinaryExpression,
  GroupingExpression,
  LiteralExpression,
  UnaryExpression
>;

template <typename Derived>
struct Expression {
  constexpr auto accept() const -> std::string_view {
    return static_cast<const Derived*>(this)->accept_impl();
  }
};

struct BinaryExpression : public Expression<BinaryExpression> {
  AnyExpression left;
  Token op;
  AnyExpression right;

  constexpr BinaryExpression(AnyExpression left, Token op, AnyExpression right)
      : left(left), op(op), right(right) {}

  constexpr auto accept_impl() const -> std::string_view {
    const auto left_val = evaluate_expression(left);
    const auto right_val = evaluate_expression(right);
    
    switch (op.get_type()) {
      case Token::Type::PLUS:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(l)>, double> && 
                         std::is_same_v<std::decay_t<decltype(r)>, double>) {
              return l + r == 0 ? "0" : "1"; // Simplified for compile-time
            } else {
              return "error";
            }
          },
          left_val, right_val
        );
      case Token::Type::MINUS:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(l)>, double> && 
                         std::is_same_v<std::decay_t<decltype(r)>, double>) {
              return l - r == 0 ? "0" : "1"; // Simplified for compile-time
            } else {
              return "error";
            }
          },
          left_val, right_val
        );
      case Token::Type::STAR:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(l)>, double> && 
                         std::is_same_v<std::decay_t<decltype(r)>, double>) {
              return l * r == 0 ? "0" : "1"; // Simplified for compile-time
            } else {
              return "error";
            }
          },
          left_val, right_val
        );
      case Token::Type::SLASH:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(l)>, double> && 
                         std::is_same_v<std::decay_t<decltype(r)>, double>) {
              return r == 0 ? "error" : (l / r == 0 ? "0" : "1");
            } else {
              return "error";
            }
          },
          left_val, right_val
        );
      case Token::Type::GREATER:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(l)>, double> && 
                         std::is_same_v<std::decay_t<decltype(r)>, double>) {
              return l > r ? "true" : "false";
            } else {
              return "error";
            }
          },
          left_val, right_val
        );
      case Token::Type::GREATER_EQUAL:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(l)>, double> && 
                         std::is_same_v<std::decay_t<decltype(r)>, double>) {
              return l >= r ? "true" : "false";
            } else {
              return "error";
            }
          },
          left_val, right_val
        );
      case Token::Type::LESS:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(l)>, double> && 
                         std::is_same_v<std::decay_t<decltype(r)>, double>) {
              return l < r ? "true" : "false";
            } else {
              return "error";
            }
          },
          left_val, right_val
        );
      case Token::Type::LESS_EQUAL:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(l)>, double> && 
                         std::is_same_v<std::decay_t<decltype(r)>, double>) {
              return l <= r ? "true" : "false";
            } else {
              return "error";
            }
          },
          left_val, right_val
        );
      case Token::Type::BANG_EQUAL:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            return l != r ? "true" : "false";
          },
          left_val, right_val
        );
      case Token::Type::EQUAL_EQUAL:
        return std::visit(
          [](auto&& l, auto&& r) -> std::string_view {
            return l == r ? "true" : "false";
          },
          left_val, right_val
        );
      default:
        return "error";
    }
  }
};

struct GroupingExpression : public Expression<GroupingExpression> {
  AnyExpression expression;

  constexpr explicit GroupingExpression(AnyExpression expression)
      : expression(expression) {}
  
  constexpr auto accept_impl() const -> std::string_view {
    return evaluate_expression(expression);
  }
};

struct LiteralExpression : public Expression<LiteralExpression> {
  TokenLiteral literal;

  constexpr explicit LiteralExpression(TokenLiteral literal) : literal(literal) {}

  constexpr auto accept_impl() const -> std::string_view {
    return std::visit(
      [](auto&& arg) -> std::string_view {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          return "nil";
        } else if constexpr (std::is_same_v<T, bool>) {
          return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, double>) {
          return arg == 0 ? "0" : "1"; // Simplified for compile-time
        } else if constexpr (std::is_same_v<T, std::string_view>) {
          return arg;
        } else {
          return "error";
        }
      },
      literal
    );
  }
};

struct UnaryExpression : public Expression<UnaryExpression> {
  Token op;
  AnyExpression right;

  constexpr UnaryExpression(Token op, AnyExpression right)
      : op(op), right(right) {}

  constexpr auto accept_impl() const -> std::string_view {
    const auto right_val = evaluate_expression(right);
    
    switch (op.get_type()) {
      case Token::Type::MINUS:
        return std::visit(
          [](auto&& arg) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, double>) {
              return -arg == 0 ? "0" : "1"; // Simplified for compile-time
            } else {
              return "error";
            }
          },
          right_val
        );
      case Token::Type::BANG:
        return std::visit(
          [](auto&& arg) -> std::string_view {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, bool>) {
              return !arg ? "true" : "false";
            } else {
              return "false";
            }
          },
          right_val
        );
      default:
        return "error";
    }
  }
};

template <typename T, typename... Args>
constexpr auto make_expression(Args&&... args) -> AnyExpression {
  return T(std::forward<Args>(args)...);
}

struct ExpressionEvaluator {
  constexpr auto operator()(const BinaryExpression& expr) const -> std::string_view {
    return expr.accept();
  }
  
  constexpr auto operator()(const GroupingExpression& expr) const -> std::string_view {
    return expr.accept();
  }
  
  constexpr auto operator()(const LiteralExpression& expr) const -> std::string_view {
    return expr.accept();
  }
  
  constexpr auto operator()(const UnaryExpression& expr) const -> std::string_view {
    return expr.accept();
  }
};

constexpr auto evaluate_expression(const AnyExpression& expr) -> std::string_view {
  return std::visit(ExpressionEvaluator{}, expr);
}
