#include "Proof.hpp"
#include "TruthTree.hpp"
#include "unordered_map"
#include <optional>

std::unordered_map<TruthNode *, std::pair<std::vector<Step>*,int>> mapping = {};

//--------------------------------//
//----- Forward Declarations -----//
//--------------------------------//

bool dnR(TruthNode* premise, std::vector<Step> *proof);
bool conR(TruthNode* premise, std::vector<Step> *proof );
bool nconR(TruthNode* premise, std::vector<Step> *proof);
bool disR(TruthNode* premise, std::vector<Step> *proof);
bool ndisR(TruthNode* premise, std::vector<Step> *proof);
bool condR(TruthNode* premise, std::vector<Step> *proof);
bool ncondR(TruthNode* premise, std::vector<Step> *proof);
bool bcondR(TruthNode* premise, std::vector<Step> *proof);
bool nbcondR(TruthNode* premise, std::vector<Step> *proof);
bool closeR(TruthNode* premise, std::vector<Step> *proof);
void checkNode(TruthNode* tnode, std::vector<Step> *proof);

//--------------------------------//
//------- Proof Functions --------//
//--------------------------------//
bool dnR(TruthNode* premise, std::vector<Step> *proof){
    //proof->push_back(Step(premise->expr.get_unnegation(),))
}

bool conR(TruthNode* premise, std::vector<Step> *proof){

}

bool nconR(TruthNode* premise, std::vector<Step> *proof){
    
}

bool disR(TruthNode* premise, std::vector<Step> *proof){
    
}

bool ndisR(TruthNode* premise, std::vector<Step> *proof){
    
}

bool condR(TruthNode* premise, std::vector<Step> *proof){
    
}

bool ncondR(TruthNode* premise, std::vector<Step> *proof){
    
}

bool bcondR(TruthNode* premise, std::vector<Step> *proof){
    
}

bool nbcondR(TruthNode* premise, std::vector<Step> *proof){
    
}

bool closeR(TruthNode* premise, std::vector<Step> *proof){
    
}

void checkNode(TruthNode *tnode, std::vector<Step> *proof){
    Step* curr = NULL;
    
    bool premis = mapping.find(tnode) != mapping.end();
    if(premis)
        curr = &(mapping.at(tnode).first->at(mapping.at(tnode).second));


    DecompositionRule::DecompositionRule rule = tnode->rule; 
    bool stop = false;
    if (!premis){
        int index = 0;
        switch (rule) {
            case DecompositionRule::DoubleNegation:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::NegationElimination;
                break;

            case DecompositionRule::Conjunction:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::ConjunctionElimination;
                break;

            case DecompositionRule::NegatedConjunction:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::Assume;
                curr->subproof = std::make_optional(std::vector<Step>());
                proof = &curr->subproof.value();
                break;

            case DecompositionRule::Disjunction:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::Assume;
                curr->subproof = std::make_optional(std::vector<Step>());
                proof = &curr->subproof.value();
                break;

            case DecompositionRule::NegatedDisjunction:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::ConjunctionElimination;
                break;

            case DecompositionRule::Conditional:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::Assume;
                curr->subproof = std::make_optional(std::vector<Step>());
                proof = &curr->subproof.value();
                break;

            case DecompositionRule::NegatedConditional:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::ConjunctionElimination;
                break;

            case DecompositionRule::Biconditional:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::Assume;
                curr->subproof = std::make_optional(std::vector<Step>());
                proof = &curr->subproof.value();
                break;
            case DecompositionRule::NegatedBiconditional:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::Assume;
                curr->subproof = std::make_optional(std::vector<Step>());
                proof = &curr->subproof.value();
                break;
            
            case DecompositionRule::Closure:
                index = proof->size();
                proof->push_back(Step(tnode->expr));
                curr = &proof->at(index);
                curr->rule = InferenceRule::ContradictionIntroduction;
                break;
            
            default:
                break;
        }

        curr->references = std::make_optional(std::vector<std::pair<std::vector<Step>*,int>>());
        curr->references.value().push_back(mapping.at(tnode->references.first));
        if(rule == DecompositionRule::Closure){
            curr->references.value().push_back(mapping.at(tnode->references.second));
        }
        mapping.insert({tnode,std::pair<std::vector<Step>*,int>(proof,index)});
    }
}

Proof to_proof(std::pair<TruthNode *, int> tt){
    
    TruthNode* root = tt.first;
    std::vector<Step> premis{};
    for (int i = 0; i < tt.second; i++){
        premis.push_back(Step(root->expr));
        mapping.insert({root, std::pair<std::vector<Step>*,int>{&premis, i}});
        root = root->children.first;
    }
    std::vector<Step> pro = {};
    int i = 0;
}
