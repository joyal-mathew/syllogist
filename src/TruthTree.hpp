#pragma once

#include "Expr.hpp"
#include <utility>

namespace DecompositionRule {
    enum DecompositionRule {
        Assume,
        DoubleNegation,
        Conjunction,
        NegatedConjunction,
        Disjunction,
        NegatedDisjunction,
        Conditional,
        NegatedConditional,
        Biconditional,
        NegatedBiconditional,
    };
}

struct TruthNode {
    Expr expr;
    DecompositionRule::DecompositionRule rule;
    std::pair<TruthNode *, TruthNode *> children;
    std::pair<TruthNode *, TruthNode *> references;
};
