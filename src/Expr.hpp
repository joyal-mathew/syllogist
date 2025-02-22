#pragma once

#include "types.hpp"

#include <ostream>
#include <utility>
#include <variant>
#include <vector>

namespace ExprType {
    enum ExprType {
        Atom,
        Contradiction,
        Negation,
        Conjunction,
        Disjunction,
        Conditional,
        Biconditional,
    };
}

class Expr;

typedef std::variant<std::pair<Expr *, Expr *>, Expr *, u16, std::monostate>
    ExprDataUnion;

class Expr {
private:
     ExprDataUnion data;

public:
    ExprType::ExprType type;

    Expr(ExprType::ExprType type, ExprDataUnion data)
        : data(data), type(type) {}

    Expr(ExprType::ExprType type) : data(std::monostate{}), type(type) {}
    Expr(ExprType::ExprType type, u16 symbol) : data(symbol), type(type) {}
    Expr(ExprType::ExprType type, Expr *operand) : data(operand), type(type) {}
    Expr(ExprType::ExprType type, Expr *lhs, Expr *rhs)
        : data(std::make_pair(lhs, rhs)), type(type) {}

    Expr(const Expr &expr);
    ~Expr();

    void negate();
    void unnegate();
    Expr *get_negation() const;
    Expr *get_unnegation() const;
    ExprType::ExprType get_unnegation_type() const;
    std::pair<Expr, Expr> decompose() const;

    friend std::ostream &operator<<(std::ostream &out, const Expr &expr);
};

std::ostream &operator<<(std::ostream &out, const Expr &expr);
