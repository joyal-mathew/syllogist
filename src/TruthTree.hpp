#pragma once

#include "Expr.hpp"
#include <utility>

namespace DecompositionRule {
    enum DecompositionRule {
        Assume,
        DoubleNegation,         // Stacking
        Conjunction,            // Stacking
        NegatedConjunction,     // Branching
        Disjunction,            // Branching
        NegatedDisjunction,     // Stacking
        Conditional,            // Branching
        NegatedConditional,     // Stacking
        Biconditional,          // Branching
        NegatedBiconditional,   // Branching
        Closure,                // Stacking
    };
}

struct TruthNode {
    Expr expr;
    DecompositionRule::DecompositionRule rule;
    std::pair<TruthNode *, TruthNode *> children;
    std::pair<TruthNode *, TruthNode *> references;

    TruthNode(Expr e, DecompositionRule::DecompositionRule  r)
        : expr(e), rule(r), children({nullptr, nullptr}), references({nullptr, nullptr}) {}
};

// Deletes the truth tree
void delete_truth_tree(TruthNode *root);

// Computes the truth tree
std::pair<TruthNode *, int> compute_truth_tree(std::vector<Expr *> premises);