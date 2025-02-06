#pragma once

#include "Clause.hpp"

#include <utility>
#include <variant>
#include <vector>

namespace ExprType {
    enum ExprType {
        Atom,
        Negation,
        Conjunction,
        Disjunction,
        Conditional,
        Biconditional,
    };
}

class Expr {
private:
    ExprType::ExprType type;
    std::variant<std::pair<Expr *, Expr *>, Expr *, u16> data;

public:
    Expr(ExprType::ExprType type, Expr *lhs, Expr *rhs)
        : type(type), data(std::make_pair(lhs, rhs)) {}

    Expr(ExprType::ExprType type, Expr *operand) : type(type), data(operand) {}

    Expr(ExprType::ExprType type, u16 symbol) : type(type), data(symbol) {}

    static Expr atom(u16 symbol) { return Expr(ExprType::Atom, symbol); }

    static Expr negation(Expr *operand) {
        return Expr(ExprType::Negation, operand);
    }

    static Expr binary(ExprType::ExprType type, Expr *lhs, Expr *rhs) {
        return Expr(type, lhs, rhs);
    }

    static Expr from_clause(Clause clause);

    void to_cnf();
    std::vector<Clause> to_clauses() const;
};
