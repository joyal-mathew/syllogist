#pragma once

#include <ostream>
#include <utility>
#include <variant>
#include <vector>

namespace ExprType {
    enum ExprType {
        Atom,
        Contradiction,
        Open_Branch,
        Negation,
        Conjunction,
        Disjunction,
        Conditional,
        Biconditional,
    };
}

class Expr;

typedef std::variant<std::pair<Expr *, Expr *>, Expr *, unsigned short, std::monostate>
    ExprDataUnion;

class Expr {
private:
     ExprDataUnion data;

public:
    ExprType::ExprType type;

    Expr(ExprType::ExprType type, ExprDataUnion data)
        : data(data), type(type) {}

    Expr(ExprType::ExprType type) : data(std::monostate{}), type(type) {}
    Expr(ExprType::ExprType type, unsigned short symbol) : data(symbol), type(type) {}
    Expr(ExprType::ExprType type, Expr *operand) : data(operand), type(type) {}
    Expr(ExprType::ExprType type, Expr *lhs, Expr *rhs)
        : data(std::make_pair(lhs, rhs)), type(type) {}

    Expr(const Expr &expr);
    ~Expr();

    unsigned short get_atom() const;
    void negate();
    void unnegate();
    Expr *get_negation() const;
    Expr *get_unnegation() const;
    Expr get_negation_v() const;
    Expr get_unnegation_v() const;
    ExprType::ExprType get_unnegation_type() const;
    std::pair<Expr, Expr> decompose() const;

    std::string to_string(bool willow = false, bool root = true);
    friend std::ostream &operator<<(std::ostream &out, const Expr &expr);
};

std::ostream &operator<<(std::ostream &out, const Expr &expr);
