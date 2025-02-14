#include "parse.hpp"
#include "Expr.hpp"
#include <cctype>

struct Operator {
    char c;
    ExprType::ExprType op;
};

const Operator OP_TABLE[] = {
    { '&', ExprType::Conjunction },
    { '|', ExprType::Disjunction },
    { '>', ExprType::Conditional },
    { '=', ExprType::Biconditional },
};

const u8 MAX_PRECEDENCE = std::extent<decltype (OP_TABLE)>::value;

struct Parser {
    const std::string &source;
    u64 index;
    char current;

    void skip_whitespace() {
        while (isspace(current)) advance();
    }

    char advance() {
        char old = current;
        ++index;
        current = index < source.length() ? source[index] : '\0';
        return old;
    }

    Expr *expression() {
        return binary_op(MAX_PRECEDENCE);
    }

    Expr *binary_op(u8 precedence) {
        if (precedence == 0) {
            return term();
        }

        u8 i = precedence - 1;

        Expr *expr = binary_op(i);
        if (!expr) return nullptr;

        for (;;) {
            skip_whitespace();

            if (current == OP_TABLE[i].c) {
                advance();
                Expr *rhs = binary_op(i);
                if (!rhs) return nullptr;
                expr = new Expr(OP_TABLE[i].op, expr, rhs);
            }
            else {
                break;
            }
        }

        return expr;
    }

    Expr *term() {
        skip_whitespace();

        char c = advance();
        switch (c) {
        case '?':
            return new Expr(ExprType::Contradiction);
        case '~':
            return negation(term());
        case '(': {
            Expr *expr = expression();
            if (!expr) return nullptr;
            skip_whitespace();

            if (current == ')') {
                advance();
                return expr;
            }

            return nullptr;
        }
        case '\0':
            return nullptr;
        default:
            return isupper(c) ? new Expr(ExprType::Atom, c) : nullptr;
        }
    }

    static Expr *negation(Expr *operand) {
        return operand ? new Expr(ExprType::Negation, operand) : nullptr;
    }
};

Expr *parse(const std::string &string) {
    if (string.length() == 0) return nullptr;
    Parser parser = {string, 0, string[0]};
    return parser.expression();
}
