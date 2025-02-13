#pragma once

#include "Expr.hpp"
#include "Resolution.hpp"

#include <optional>
#include <vector>

namespace InferenceRule {
    enum InferenceRule {
        Assume,
        Reiteration,
        ContradictionIntroduction,
        ContradictionElimination,
        NegationIntroduction,
        NegationElimination,
        ConjunctionIntroduction,
        ConjunctionElimination,
        DisjunctionIntroduction,
        DisjunctionElimination,
        ConditionalIntroduction,
        ConditionalElimination,
        BiconditionalIntroduction,
        BiconditionalElimination,
    };
}

struct Step {
    Expr expr;
    InferenceRule::InferenceRule rule;
    std::vector<Step *> citations;
    std::optional<std::vector<Step>> subproof;
};

struct Proof {
    std::vector<Step> premises;
    std::vector<Step> proof;
};

Proof to_proof(const ResolutionContext &resolution_context);
