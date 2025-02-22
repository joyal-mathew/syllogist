#pragma once

#include "Expr.hpp"
#include <utility>

namespace DecompositionRule {
    enum DecompositionRule {
        Assume,
        DoubleNegation,         // Stacking     // Works
        Conjunction,            // Stacking 
        NegatedConjunction,     // Branching 
        Disjunction,            // Branching    // Works
        NegatedDisjunction,     // Stacking
        Conditional,            // Branching
        NegatedConditional,     // Stacking     // Works
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

    TruthNode(Expr e, DecompositionRule::DecompositionRule d)
        : expr(e), rule(d), children({nullptr, nullptr}), references({nullptr, nullptr}) {}
    
    TruthNode(Expr e, DecompositionRule::DecompositionRule d, TruthNode* r)
        : expr(e), rule(d), children({nullptr, nullptr}), references({r, nullptr}) {}
    
    // Cursed copy constructor
    TruthNode(const TruthNode& other)
        : expr(other.expr),
          rule(other.rule),
          children({other.children.first ? new TruthNode(*other.children.first) : nullptr, other.children.second ? new TruthNode(*other.children.second) : nullptr}),
          references(other.references) {}

};

// Outputs truth tree as a dot file
void export_truth_tree_to_dot(TruthNode *root);

// Deletes the truth tree
void delete_truth_tree(TruthNode *root);

// Computes the truth tree
std::pair<TruthNode *, int> compute_truth_tree(std::vector<Expr *> premises);