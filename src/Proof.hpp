#pragma once

#include "Expr.hpp"
#include "TruthTree.hpp"
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
    std::optional<std::vector<Step *>> references;
    std::optional<std::vector<Step>> subproof;

    Step(Expr e)
        : expr(e), rule(InferenceRule::Assume), references(), subproof(){}

    Step(Expr e, InferenceRule::InferenceRule r, std::vector<Step *> ref)
        : expr(e), rule(r), references(ref), subproof(){}
};

struct Proof {
    std::vector<Step> premises;
    std::vector<Step> proof;
    Proof(std::vector<Step> pre, std::vector<Step> pro)
        : premises(pre), proof(pro){}
};

Proof to_proof(std::pair<TruthNode *, int> tt);
