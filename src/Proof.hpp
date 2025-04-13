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
    std::optional<std::vector<std::pair<std::vector<Step>*,int>>> references;
    std::optional<std::vector<Step>> subproof;
    int line_number;

    Step(Expr e, bool sp = true)
        : expr(e), rule(InferenceRule::Assume){
            if(sp){
                subproof = std::make_optional(std::vector<Step>());
            }
        }


    Step(Expr e, InferenceRule::InferenceRule r, std::pair<std::vector<Step>*,int> sl)
        : expr(e), rule(r){
            references = std::make_optional(std::vector<std::pair<std::vector<Step>*,int>>());
            references.value().push_back(sl);
        }

    Step(const Step &step) : expr(step.expr), rule(step.rule), references(step.references), subproof(step.subproof) {}
};

typedef std::pair<std::vector<Step>*,int> StepLoc;

struct Proof {
    std::vector<Step> premises;
    std::vector<Step> proof;
    Proof(){
        premises = {};
        proof = {};
    }
    Proof(std::vector<Step> pre, std::vector<Step> pro)
        : premises(pre), proof(pro){}
};

Proof to_proof(std::pair<TruthNode *, int> tt);
