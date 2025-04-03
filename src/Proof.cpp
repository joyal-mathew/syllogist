#include "Proof.hpp"
#include "TruthTree.hpp"
#include "unordered_map"


//--------------------------------//
//----- Forward Declarations -----//
//--------------------------------//

bool dnR(TruthNode *curr, std::vector<Step> *proof);
bool conR(TruthNode *curr, std::vector<Step> *proof);
bool nconR(TruthNode *curr, std::vector<Step> *proof);
bool disR(TruthNode *curr, std::vector<Step> *proof);
bool ndisR(TruthNode *curr, std::vector<Step> *proof);
bool condR(TruthNode *curr, std::vector<Step> *proof);
bool ncondR(TruthNode *curr, std::vector<Step> *proof);
bool bcondR(TruthNode *curr, std::vector<Step> *proof);
bool nbcondR(TruthNode *curr, std::vector<Step> *proof);
bool closeR(TruthNode *curr, std::vector<Step> *proof);
void checkNode(TruthNode *curr, std::vector<Step> *proof);

//--------------------------------//
//------- Proof Functions --------//
//--------------------------------//
bool dnR(TruthNode *curr, std::vector<Step> *proof){
    std::vector<Step *> ref = {};
    
    Step Step(curr->children.first->expr, InferenceRule::NegationElimination, ref);

}

bool conR(TruthNode *curr, std::vector<Step> *proof){
    
}

bool nconR(TruthNode *curr, std::vector<Step> *proof){
    
}

bool disR(TruthNode *curr, std::vector<Step> *proof){
    
}

bool ndisR(TruthNode *curr, std::vector<Step> *proof){
    
}

bool condR(TruthNode *curr, std::vector<Step> *proof){
    
}

bool ncondR(TruthNode *curr, std::vector<Step> *proof){
    
}

bool bcondR(TruthNode *curr, std::vector<Step> *proof){
    
}

bool nbcondR(TruthNode *curr, std::vector<Step> *proof){
    
}

bool closeR(TruthNode *curr, std::vector<Step> *proof){
    
}

void checkNode(TruthNode *curr, std::vector<Step> *proof){
    DecompositionRule::DecompositionRule rule = curr->rule; 
    bool stop = false;
    switch (rule) {
        case DecompositionRule::DoubleNegation:
            stop = dnR(curr,proof);
            break;

        case DecompositionRule::Conjunction:
            stop = conR(curr,proof);
            break;

        case DecompositionRule::NegatedConjunction:
            stop = nconR(curr,proof);
            break;

        case DecompositionRule::Disjunction:
            stop = disR(curr,proof);
            break;

        case DecompositionRule::NegatedDisjunction:
            stop = ndisR(curr,proof);
            break;

         case DecompositionRule::Conditional:
            stop = condR(curr,proof);
            break;

        case DecompositionRule::NegatedConditional:
            stop = ncondR(curr,proof);
            break;

        case DecompositionRule::Biconditional:
            stop = bcondR(curr,proof);
            break;
        
        case DecompositionRule::NegatedBiconditional:
            stop = nbcondR(curr,proof);
            break;
        
        case DecompositionRule::Closure:
            stop = closeR(curr,proof);
            break;
        
        default:
            break;
    }
}

Proof to_proof(std::pair<TruthNode *, int> tt){
    std::unordered_map<TruthNode *, std::pair<std::vector<Step>*,int>> mapping = {};
    TruthNode* root = tt.first;
    std::vector<Step> premis = {};
    for (int i = 0; i < tt.second; i++){
        premis.push_back(Step(root->expr));
        mapping.insert({root, std::pair<std::vector<Step>*,int>{&premis, i}});
        root = root->children.first;
    }
    std::vector<Step> pro = {};
    int i = 0;
}
