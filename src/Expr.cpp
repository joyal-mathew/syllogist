#include "Expr.hpp"
#include "errors.hpp"
#include <utility>

Expr::Expr(const Expr &expr) {
    type = expr.type;

    switch (expr.type) {
    case ExprType::Atom:
        data = std::get<2>(expr.data);
        break;
    case ExprType::Contradiction:
        break;
    case ExprType::Open_Branch:
        break;
    case ExprType::Negation:
        data = new Expr(*std::get<1>(expr.data));
        break;
    default:
        data = std::make_pair(new Expr(*std::get<0>(expr.data).first),
                              new Expr(*std::get<0>(expr.data).second));
        break;
    }
}

Expr::~Expr() {
    switch (type) {
    case ExprType::Atom:
    case ExprType::Contradiction:
        break;
    case ExprType::Negation:
        delete std::get<1>(data);
        break;
    default:
        delete std::get<0>(data).first;
        delete std::get<0>(data).second;
    }
}

u16 Expr::get_atom() const {
    if (type == ExprType::Atom)
        return std::get<2>(data);
    if (type == ExprType::Negation)
        return std::get<1>(data)->get_atom();
    DEBUG_ASSERT(false);
}

void Expr::negate() {
    Expr *oprand = new Expr(type, data);
    type = ExprType::Negation;
    data = oprand;
}

void Expr::unnegate() {
    DEBUG_ASSERT(type == ExprType::Negation);
    Expr *oprand = std::get<1>(data);
    type = oprand->type;
    data = oprand->data;
    oprand->type = ExprType::Contradiction;
    delete oprand;
}

Expr *Expr::get_negation() const {
    Expr *expr = new Expr(*this);
    expr->negate();
    return expr;
}

Expr *Expr::get_unnegation() const {
    Expr *expr = new Expr(*this);
    expr->unnegate();
    return expr;
}

ExprType::ExprType Expr::get_unnegation_type() const {
    DEBUG_ASSERT(type == ExprType::Negation);
    return std::get<1>(data)->type;
}

std::pair<Expr, Expr> Expr::decompose() const {
    using namespace ExprType;
    DEBUG_ASSERT(type == Conjunction || type == Disjunction ||
                 type == Conditional || type == Biconditional);
    Expr lhs = Expr(*std::get<0>(data).first);
    Expr rhs = Expr(*std::get<0>(data).second);
    return std::make_pair(lhs, rhs);
}

std::string Expr::to_string() {
    switch (type) {
        case ExprType::Atom:
            return std::string(1, std::get<2>(data));
        case ExprType::Contradiction:
            return "⊥";
        case ExprType::Negation:
            return "¬("+std::get<1>(data)->to_string()+")";
        case ExprType::Conjunction:
            return "("+std::get<0>(data).first->to_string()+"∧"+std::get<0>(data).second->to_string()+")";
        case ExprType::Disjunction:
            return "("+std::get<0>(data).first->to_string()+"∨"+std::get<0>(data).second->to_string()+")";
        case ExprType::Conditional:
            return "("+std::get<0>(data).first->to_string()+"→"+std::get<0>(data).second->to_string()+")";
        case ExprType::Biconditional:
            return "("+std::get<0>(data).first->to_string()+"↔"+std::get<0>(data).second->to_string()+")";
    }
    return "";
}

struct BinaryOperatorEntry {
    ExprType::ExprType op;
    const char *word;
};

const BinaryOperatorEntry BIN_OP_TABLE[] = {
    { ExprType::Conjunction, "and" },
    { ExprType::Disjunction, "or" },
    { ExprType::Conditional, "cond" },
    { ExprType::Biconditional, "bicond" },
};

const unsigned BIN_OP_TABLE_LEN = std::extent<decltype (BIN_OP_TABLE)>::value;

std::ostream &operator<<(std::ostream &out, const Expr &expr) {
    switch (expr.type) {
    case ExprType::Atom:
        out << (char) std::get<2>(expr.data);
        break;
    case ExprType::Contradiction:
        out << '?';
        break;
    case ExprType::Open_Branch:
        out << '!';
        break;
    case ExprType::Negation:
        out << "(not " << *std::get<1>(expr.data) << ')';
        break;
    default:
        for (unsigned i = 0; i < BIN_OP_TABLE_LEN; ++i) {
            if (BIN_OP_TABLE[i].op == expr.type) {
                out << '(' << BIN_OP_TABLE[i].word << ' '
                    << *std::get<0>(expr.data).first << ' '
                    << *std::get<0>(expr.data).second << ')';
                return out;
            }
        }

        ASSERT(false);
    }

    return out;
}
