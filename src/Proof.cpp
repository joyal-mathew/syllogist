#include "Proof.hpp"
#include "TruthTree.hpp"
#include "unordered_map"
#include <optional>


typedef std::pair<std::vector<Step>*,int> StepLoc;

std::unordered_map<TruthNode *, StepLoc> mapping = {};

//--------------------------------//
//----- Forward Declarations -----//
//--------------------------------//

void dnR(TruthNode* assume, std::vector<Step> *proof);
//void conR(TruthNode* assume, std::vector<Step> *proof );
void nconR(TruthNode* assume, std::vector<Step> *proof);
void ndisR(TruthNode* assume, std::vector<Step> *proof);
void condR(TruthNode* assume, std::vector<Step> *proof);
void ncondR(TruthNode* assume, std::vector<Step> *proof);
void bcondR(TruthNode* assume, std::vector<Step> *proof);
void nbcondR(TruthNode* assume, std::vector<Step> *proof);
void closeR(TruthNode* assume, std::vector<Step> *proof);
void checkNode(TruthNode* tnode, std::vector<Step> *proof);

//--------------------------------//
//------- Proof Functions --------//
//--------------------------------//
void dnR(TruthNode* assume, std::vector<Step> *proof){
    proof->push_back(Step(*assume->expr.get_unnegation(),InferenceRule::NegationElimination,mapping.at(assume)));
    mapping.at(assume) = StepLoc{proof,proof->size()-1};
}

// void conR(TruthNode* assume, std::vector<Step> *proof){
//     std::pair<Expr, Expr> decomposed = assume->expr.decompose();
//     proof->push_back(Step(decomposed.first,InferenceRule::ConjunctionElimination,mapping.at(assume)));
//     proof->push_back( Step(decomposed.second,InferenceRule::ConjunctionElimination,mapping.at(assume)));
// }

void nconR(TruthNode* assume, std::vector<Step> *proof){
    std::pair<Expr, Expr> decomposed = assume->expr.get_unnegation()->decompose();
    Expr *first_copy = new Expr(decomposed.first);
    Expr *second_copy = new Expr(decomposed.second);
    Expr goal =  Expr(ExprType::Disjunction,
                    decomposed.first.get_negation(),
                    decomposed.second.get_negation());

    Step second_assume = Step(*goal.get_negation());
    proof->push_back(second_assume);
    std::vector<Step>* second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        Step third_assume = Step(*decomposed.first.get_negation());
        second_layer->push_back(third_assume);
        std::vector<Step>* third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            third_layer->push_back(Step(
                goal,
                InferenceRule::DisjunctionIntroduction,
                StepLoc{second_layer, second_layer->size()-1}
            ));
            third_layer->push_back(Step(
                Expr(ExprType::Contradiction),
                InferenceRule::ContradictionIntroduction,
                StepLoc{proof, proof->size()-1}
            ));
            third_layer->at(third_layer->size()-1).references.value().push_back(StepLoc{third_layer,0});
        //----------------------------------------------------------------    
        second_layer->push_back(Step(
            *(*decomposed.first.get_negation()).get_negation(),
            InferenceRule::NegationIntroduction,
            StepLoc{second_layer,0}
        ));
        second_layer->push_back(Step(
            decomposed.first,
            InferenceRule::NegationElimination,
            StepLoc{second_layer,1}
        ));
        third_assume = Step(*decomposed.second.get_negation());
        second_layer->push_back(third_assume);
        third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            third_layer->push_back(Step(
                goal,
                InferenceRule::DisjunctionIntroduction,
                StepLoc{second_layer, second_layer->size()-1}
            ));
            third_layer->push_back(Step(
                Expr(ExprType::Contradiction),
                InferenceRule::ContradictionIntroduction,
                StepLoc{proof, proof->size()-1}
            ));
            third_layer->at(third_layer->size()-1).references.value().push_back(StepLoc{third_layer,0});
        //----------------------------------------------------------------    
        second_layer->push_back(Step(
            *(*decomposed.second.get_negation()).get_negation(),
            InferenceRule::NegationIntroduction,
            StepLoc{second_layer,3}
        ));
        second_layer->push_back(Step(
            decomposed.second,
            InferenceRule::NegationElimination,
            StepLoc{second_layer,4}
        ));
        second_layer->push_back(Step(
            Expr(ExprType::Disjunction,first_copy,second_copy),
            InferenceRule::ConjunctionIntroduction,
            StepLoc{second_layer,2}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,5});
        second_layer->push_back(Step(
            Expr(ExprType::Contradiction),
            InferenceRule::ContradictionIntroduction,
            StepLoc{proof, proof->size()-2}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,6});
    //----------------------------------------------------------------    
    proof->push_back(Step(
        *second_assume.expr.get_negation(),
        InferenceRule::NegationIntroduction,
        StepLoc{proof,proof->size()-1}
    ));
    proof->push_back(Step(
        goal,
        InferenceRule::NegationElimination,
        StepLoc{proof,proof->size()-1}
    ));
    mapping.at(assume) = StepLoc{proof,proof->size()-1};
}


void ndisR(TruthNode* assume, std::vector<Step> *proof){
    std::pair<Expr, Expr> decomposed = assume->expr.get_unnegation()->decompose();
    Step second_assume = Step(decomposed.first);
    proof->push_back(second_assume);
    std::vector<Step>* second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        second_layer->push_back(Step(
            *assume->expr.get_unnegation(),
            InferenceRule::DisjunctionIntroduction,
            StepLoc{second_layer, second_layer->size()-1}
        ));
        second_layer->push_back(Step(
            Expr(ExprType::Contradiction),
            InferenceRule::ContradictionIntroduction,
            mapping.at(assume)
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,0});
    //----------------------------------------------------------------    
    proof->push_back(Step(
        *decomposed.first.get_negation(),
        InferenceRule::NegationIntroduction,
        StepLoc{proof,proof->size()-1}
    ));
    second_assume = Step(decomposed.second);
    proof->push_back(second_assume);
    second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        second_layer->push_back(Step(
            *assume->expr.get_unnegation(),
            InferenceRule::DisjunctionIntroduction,
            StepLoc{second_layer, second_layer->size()-1}
        ));
        second_layer->push_back(Step(
            Expr(ExprType::Contradiction),
            InferenceRule::ContradictionIntroduction,
            mapping.at(assume)
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,0});
    //----------------------------------------------------------------    
    proof->push_back(Step(
        *decomposed.first.get_negation(),
        InferenceRule::NegationIntroduction,
        StepLoc{proof,proof->size()-1}
    ));       
    proof->push_back(Step(
        Expr(ExprType::Conjunction,
            decomposed.first.get_negation(),
            decomposed.second.get_negation()),
        InferenceRule::ConjunctionIntroduction,
        StepLoc{proof,proof->size()-3}
    )); 
    proof->at(proof->size()-1).references.value().push_back(StepLoc{proof,proof->size()-2});
    mapping.at(assume) = StepLoc{proof,proof->size()-1};
}

void condR(TruthNode* assume, std::vector<Step> *proof){
    std::pair<Expr, Expr> decomposed = assume->expr.decompose();
    Expr *first_copy = new Expr(decomposed.first);
    Expr *second_copy = new Expr(decomposed.second);
    Expr goal =  Expr(ExprType::Disjunction,
        first_copy,
        decomposed.first.get_negation());
        
    Step second_assume = Step(*goal.get_negation());
    proof->push_back(second_assume);
    std::vector<Step>* second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        Step third_assume = Step(decomposed.first);
        second_layer->push_back(third_assume);
        std::vector<Step>* third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            third_layer->push_back(Step(
                goal,
                InferenceRule::DisjunctionIntroduction,
                StepLoc{second_layer, second_layer->size()-1}
            ));
            third_layer->push_back(Step(
                Expr(ExprType::Contradiction),
                InferenceRule::ContradictionIntroduction,
                StepLoc{proof, proof->size()-1}
            ));
            third_layer->at(third_layer->size()-1).references.value().push_back(StepLoc{third_layer,0});
        //----------------------------------------------------------------    
        second_layer->push_back(Step(
            *decomposed.first.get_negation(),
            InferenceRule::NegationIntroduction,
            StepLoc{second_layer,second_layer->size()-1}
        ));
        second_layer->push_back(Step(
            goal,
            InferenceRule::DisjunctionIntroduction,
            StepLoc{second_layer, second_layer->size()-1}
        ));
        second_layer->push_back(Step(
            Expr(ExprType::Contradiction),
            InferenceRule::ContradictionIntroduction,
            StepLoc{proof, proof->size()-1}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
    //----------------------------------------------------------------    
    proof->push_back(Step(
        *(*goal.get_negation()).get_negation(),
        InferenceRule::NegationIntroduction,
        StepLoc{proof,proof->size()-1}
    ));
    proof->push_back(Step(
        goal,
        InferenceRule::NegationElimination,
        StepLoc{proof,proof->size()-1}
    ));
    goal =  Expr(ExprType::Disjunction,
        decomposed.first.get_negation(),
        second_copy);
    second_assume = Step(*goal.get_negation());
    proof->push_back(second_assume);
    second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        third_assume = Step(decomposed.first);
        second_layer->push_back(third_assume);
         third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            third_layer->push_back(Step(
                decomposed.second,
                InferenceRule::ConditionalElimination,
                mapping.at(assume)
            ));
            third_layer->at(third_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-1});
            third_layer->push_back(Step(
                goal,
                InferenceRule::DisjunctionIntroduction,
                StepLoc{third_layer, 0}
            ));    
        //----------------------------------------------------------------    
        third_assume = Step(*decomposed.first.get_negation());
        second_layer->push_back(third_assume);
         third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            third_layer->push_back(Step(
                goal,
                InferenceRule::DisjunctionIntroduction,
                StepLoc{second_layer, second_layer->size()-1}
            ));    
        //----------------------------------------------------------------    
        second_layer->push_back(Step(
            goal,
            InferenceRule::DisjunctionElimination,
            StepLoc{proof,proof->size()-2}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-3});
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
        second_layer->push_back(Step(
            Expr(ExprType::Contradiction),
            InferenceRule::ContradictionIntroduction,
            StepLoc{proof,proof->size()-1}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
    //----------------------------------------------------------------     
    proof->push_back(Step(
        *(*goal.get_negation()).get_negation(),
        InferenceRule::NegationIntroduction,
        StepLoc{proof,proof->size()-1}
    ));
    proof->push_back(Step(
        goal,
        InferenceRule::NegationElimination,
        StepLoc{proof,proof->size()-1}
    ));
    mapping.at(assume) = StepLoc{proof,proof->size()-1};
}

void ncondR(TruthNode* assume, std::vector<Step> *proof){
    std::pair<Expr, Expr> decomposed = (*assume->expr.get_unnegation()).decompose();
    Expr *first_copy = new Expr(decomposed.first);
    Expr goal =  Expr(ExprType::Conjunction,
        first_copy,
        decomposed.second.get_negation());
    
    Step second_assume = Step(*goal.get_negation());
    proof->push_back(second_assume);
    std::vector<Step>* second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        Step third_assume = Step(decomposed.first);
        second_layer->push_back(third_assume);
        std::vector<Step>* third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            Step fourth_assume = Step(*decomposed.second.get_negation());
            third_layer->push_back(fourth_assume);
            std::vector<Step>* fourth_layer = &fourth_assume.subproof.value();
                //----------------------------------------------------------------
                fourth_layer->push_back(Step(
                    goal,
                    InferenceRule::ConjunctionIntroduction,
                    StepLoc{second_layer, second_layer->size()-1}
                ));
                fourth_layer->at(fourth_layer->size()-1).references.value().push_back(StepLoc{third_layer,0});
                fourth_layer->push_back(Step(
                    Expr(ExprType::Contradiction),
                    InferenceRule::ContradictionIntroduction,
                    StepLoc{proof, proof->size()-1}
                ));
                fourth_layer->at(fourth_layer->size()-1).references.value().push_back(StepLoc{fourth_layer,0});
            //----------------------------------------------------------------    
            third_layer->push_back(Step(
                *(*decomposed.second.get_negation()).get_negation(),
                InferenceRule::NegationIntroduction,
                StepLoc{third_layer,third_layer->size()-1}
            ));
            third_layer->push_back(Step(
                decomposed.second,
                InferenceRule::NegationElimination,
                StepLoc{third_layer, third_layer->size()-1}
            ));
        //----------------------------------------------------------------    
        second_layer->push_back(Step(
            *assume->expr.get_unnegation(),
            InferenceRule::ConditionalIntroduction,
            StepLoc{second_layer,second_layer->size()-1}
        ));
        second_layer->push_back(Step(
            goal,
            InferenceRule::DisjunctionIntroduction,
            mapping.at(assume)
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
    //----------------------------------------------------------------  
    proof->push_back(Step(
        *(*goal.get_negation()).get_negation(),
        InferenceRule::NegationIntroduction,
        StepLoc{proof,proof->size()-1}
    ));    
    proof->push_back(Step(
        goal,
        InferenceRule::NegationElimination,
        StepLoc{proof, proof->size()-1}
    ));
    mapping.at(assume) = StepLoc{proof,proof->size()-1};
}

void bcondR(TruthNode* assume, std::vector<Step> *proof){
    std::pair<Expr, Expr> decomposed = assume->expr.decompose();
    Expr *first_copy = new Expr(decomposed.first);
    Expr *second_copy = new Expr(decomposed.second);
    Expr goal =  Expr(ExprType::Disjunction,
        first_copy,
        decomposed.first.get_negation());
        
    Step second_assume = Step(*goal.get_negation());
    proof->push_back(second_assume);
    std::vector<Step>* second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        Step third_assume = Step(decomposed.first);
        second_layer->push_back(third_assume);
        std::vector<Step>* third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            third_layer->push_back(Step(
                goal,
                InferenceRule::DisjunctionIntroduction,
                StepLoc{second_layer, second_layer->size()-1}
            ));
            third_layer->push_back(Step(
                Expr(ExprType::Contradiction),
                InferenceRule::ContradictionIntroduction,
                StepLoc{proof, proof->size()-1}
            ));
            third_layer->at(third_layer->size()-1).references.value().push_back(StepLoc{third_layer,0});
        //----------------------------------------------------------------    
        second_layer->push_back(Step(
            *decomposed.first.get_negation(),
            InferenceRule::NegationIntroduction,
            StepLoc{second_layer,second_layer->size()-1}
        ));
        second_layer->push_back(Step(
            goal,
            InferenceRule::DisjunctionIntroduction,
            StepLoc{second_layer, second_layer->size()-1}
        ));
        second_layer->push_back(Step(
            Expr(ExprType::Contradiction),
            InferenceRule::ContradictionIntroduction,
            StepLoc{proof, proof->size()-1}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
    //----------------------------------------------------------------    
    proof->push_back(Step(
        *(*goal.get_negation()).get_negation(),
        InferenceRule::NegationIntroduction,
        StepLoc{proof,proof->size()-1}
    ));
    proof->push_back(Step(
        goal,
        InferenceRule::NegationElimination,
        StepLoc{proof,proof->size()-1}
    )); 
    Expr* goal_L = new Expr(ExprType::Conjunction,
        first_copy,
        second_copy
    );
    Expr* goal_R = new Expr(ExprType::Conjunction,
        first_copy,
        second_copy
    );
    Expr goal = Expr (
        ExprType::Disjunction,
        goal_L,
        goal_R
    );
    second_assume = Step(decomposed.first);
    proof->push_back(second_assume);
    std::vector<Step>* second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        second_layer->push_back(Step(
            decomposed.second,
            InferenceRule::BiconditionalElimination,
            mapping.at(assume)
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{proof,proof->size()-1});
        second_layer->push_back(Step(
            Expr(ExprType::Conjunction, first_copy,second_copy),
            InferenceRule::ConjunctionIntroduction,
            StepLoc{proof,proof->size()-1}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
        second_layer->push_back(Step(
            goal,
            InferenceRule::DisjunctionIntroduction,
            StepLoc{second_layer,second_layer->size()-1}
        ));
    //----------------------------------------------------------------
    second_assume = Step(*decomposed.first.get_negation());
    proof->push_back(second_assume);
    std::vector<Step>* second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------
        third_assume = Step(decomposed.second);
        second_layer->push_back(third_assume);
         third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            third_layer->push_back(Step(
                decomposed.first,
                InferenceRule::BiconditionalElimination,
                mapping.at(assume)
            ));
            third_layer->at(third_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-1});
            third_layer->push_back(Step(
                Expr(ExprType::Contradiction),
                InferenceRule::ContradictionIntroduction,
                StepLoc{proof, proof->size()-1}
            ));
            third_layer->at(third_layer->size()-1).references.value().push_back(StepLoc{third_layer,0});
        //----------------------------------------------------------------    
        second_layer->push_back(Step(
            *decomposed.second.get_negation(),
            InferenceRule::NegationIntroduction,
            StepLoc{second_layer, second_layer->size()-1}
        ));
        second_layer->push_back(Step(
            Expr(ExprType::Conjunction, decomposed.first.get_negation(),decomposed.second.get_negation()),
            InferenceRule::ConjunctionIntroduction,
            StepLoc{proof,proof->size()-1}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
        second_layer->push_back(Step(
            goal,
            InferenceRule::DisjunctionIntroduction,
            StepLoc{second_layer,second_layer->size()-1}
        ));
    //----------------------------------------------------------------
    proof->push_back(Step(
        goal,
        InferenceRule::DisjunctionElimination,
        StepLoc{proof,proof->size()-3}
    ));
    proof->at(proof->size()-1).references.value().push_back(StepLoc{proof,proof->size()-3});
    proof->at(proof->size()-1).references.value().push_back(StepLoc{proof,proof->size()-2});
    mapping.at(assume) = StepLoc{proof,proof->size()-1};
}

void nbcondR(TruthNode* assume, std::vector<Step> *proof){
    std::pair<Expr, Expr> decomposed = (*assume->expr.get_unnegation()).decompose();
    Expr *first_copy = new Expr(decomposed.first);
    Expr *second_copy = new Expr(decomposed.second);
    Expr *first_goal = new Expr(ExprType::Conjunction,first_copy,decomposed.second.get_negation());
    Expr *second_goal = new Expr(ExprType::Conjunction,decomposed.first.get_negation(),second_copy);
    Expr goal =  Expr(ExprType::Disjunction,
        first_goal,
        second_goal
    );
    Step second_assume = Step(*goal.get_negation());
    proof->push_back(second_assume);
    std::vector<Step>* second_layer = &proof->at(proof->size()-1).subproof.value();
        //----------------------------------------------------------------      
        Step third_assume = Step(decomposed.first);
        second_layer->push_back(third_assume);
        std::vector<Step>* third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            Step fourth_assume = Step(*decomposed.second.get_negation());
            third_layer->push_back(fourth_assume);
            std::vector<Step>* fourth_layer = &third_layer->at(third_layer->size()-1).subproof.value();
                //----------------------------------------------------------------
                fourth_layer->push_back(Step(
                    *first_goal,
                    InferenceRule::ConjunctionIntroduction,
                    StepLoc{second_layer,second_layer->size()-1}
                ));
                fourth_layer->at(fourth_layer->size()-1).references.value().push_back(StepLoc{third_layer,third_layer->size()-1});
                fourth_layer->push_back(Step(
                    goal,
                    InferenceRule::DisjunctionIntroduction,
                    StepLoc{fourth_layer,fourth_layer->size()-1}
                ));
                fourth_layer->push_back(Step(
                    Expr(ExprType::Contradiction),
                    InferenceRule::ContradictionIntroduction,
                    StepLoc{proof, proof->size()-1}
                ));
                fourth_layer->at(fourth_layer->size()-1).references.value().push_back(StepLoc{fourth_layer,fourth_layer->size()-2});
            //----------------------------------------------------------------  
            third_layer->push_back(Step(
                *(*decomposed.second.get_negation()).get_negation(),
                InferenceRule::NegationIntroduction,
                StepLoc{third_layer,third_layer->size()-1}
            ));
            third_layer->push_back(Step(
                decomposed.second,
                InferenceRule::NegationElimination,
                StepLoc{third_layer, third_layer->size()-1}
            ));
        //----------------------------------------------------------------    
        third_assume = Step(decomposed.second);
        second_layer->push_back(third_assume);
        third_layer = &second_layer->at(second_layer->size()-1).subproof.value();
            //----------------------------------------------------------------
            fourth_assume = Step(*decomposed.first.get_negation());
            third_layer->push_back(fourth_assume);
            fourth_layer = &third_layer->at(third_layer->size()-1).subproof.value();
                //----------------------------------------------------------------
                fourth_layer->push_back(Step(
                    *second_goal,
                    InferenceRule::ConjunctionIntroduction,
                    StepLoc{second_layer,second_layer->size()-1}
                ));
                fourth_layer->at(fourth_layer->size()-1).references.value().push_back(StepLoc{third_layer,third_layer->size()-1});
                fourth_layer->push_back(Step(
                    goal,
                    InferenceRule::DisjunctionIntroduction,
                    StepLoc{fourth_layer,fourth_layer->size()-1}
                ));
                fourth_layer->push_back(Step(
                    Expr(ExprType::Contradiction),
                    InferenceRule::ContradictionIntroduction,
                    StepLoc{proof, proof->size()-1}
                ));
                fourth_layer->at(fourth_layer->size()-1).references.value().push_back(StepLoc{fourth_layer,fourth_layer->size()-2});
            //----------------------------------------------------------------  
            third_layer->push_back(Step(
                *(*decomposed.first.get_negation()).get_negation(),
                InferenceRule::NegationIntroduction,
                StepLoc{third_layer,third_layer->size()-1}
            ));
            third_layer->push_back(Step(
                decomposed.first,
                InferenceRule::NegationElimination,
                StepLoc{third_layer, third_layer->size()-1}
            ));
        //---------------------------------------------------------------- 
        second_layer->push_back(Step(
            *assume->expr.get_unnegation(),
            InferenceRule::BiconditionalIntroduction,
            StepLoc{second_layer,second_layer->size()-2}
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
        second_layer->push_back(Step(
            Expr(ExprType::Contradiction),
            InferenceRule::ContradictionIntroduction,
            mapping.at(assume)
        ));
        second_layer->at(second_layer->size()-1).references.value().push_back(StepLoc{second_layer,second_layer->size()-2});
    //----------------------------------------------------------------     
    proof->push_back(Step(
        *(*goal.get_negation()).get_negation(),
        InferenceRule::NegationIntroduction,
        StepLoc{proof,proof->size()-1}
    ));
    proof->push_back(Step(
        goal,
        InferenceRule::NegationElimination,
        StepLoc{proof,proof->size()-1}
    ));
    mapping.at(assume) = StepLoc{proof,proof->size()-1};
}   

// void closeR(TruthNode* assume, std::vector<Step> *proof){
    
// }

void checkNode(TruthNode *tnode, std::vector<Step> *proof){
   
    
    bool premis = mapping.find(tnode) != mapping.end();

    DecompositionRule::DecompositionRule rule = tnode->rule; 
    if (!premis){
        mapping.insert({tnode,StepLoc(proof,proof->size())});
        switch (rule) {
            case DecompositionRule::DoubleNegation:
                proof->push_back(Step(tnode->expr,
                    InferenceRule::NegationElimination,
                    mapping.at(tnode->references.first)
                ));
                break;

            case DecompositionRule::Conjunction:
                proof->push_back(Step(tnode->expr,
                    InferenceRule::ConjunctionElimination,
                    mapping.at(tnode->references.first)
                 ));
                break;

            case DecompositionRule::NegatedConjunction:
                proof->push_back(Step(tnode->expr));
                proof = &proof->at(proof->size()-1).subproof.value();
                break;

            case DecompositionRule::Disjunction:
                proof->push_back(Step(tnode->expr));
                proof = &proof->at(proof->size()-1).subproof.value();
                break;

            case DecompositionRule::NegatedDisjunction:
                proof->push_back(Step(tnode->expr,
                    InferenceRule::ConjunctionElimination,
                    mapping.at(tnode->references.first)
                 ));
                break;

            case DecompositionRule::Conditional:
                proof->push_back(Step(tnode->expr));
                proof = &proof->at(proof->size()-1).subproof.value();
                break;

            case DecompositionRule::NegatedConditional:
                proof->push_back(Step(tnode->expr,
                    InferenceRule::ConjunctionElimination,
                    mapping.at(tnode->references.first)
                 ));
                break;

            case DecompositionRule::Biconditional:
                proof->push_back(Step(tnode->expr));
                proof = &proof->at(proof->size()-1).subproof.value();
                break;
            case DecompositionRule::NegatedBiconditional:
                proof->push_back(Step(tnode->expr));
                proof = &proof->at(proof->size()-1).subproof.value();
                break;
            
            case DecompositionRule::Closure:
                proof->push_back(Step(tnode->expr,
                    InferenceRule::ContradictionIntroduction,
                    mapping.at(tnode->references.first)
                ));
                proof->at(proof->size()-1).references.value().push_back(mapping.at(tnode->references.second));
                break;
            
            default:
                break;
        }
       
    }
    Step* curr = &(mapping.at(tnode).first->at(mapping.at(tnode).second));
}

Proof to_proof(std::pair<TruthNode *, int> tt){
    
    TruthNode* root = tt.first;
    std::vector<Step> premis{};
    for (int i = 0; i < tt.second; i++){
        premis.push_back(Step(root->expr,false));
        mapping.insert({root, StepLoc{&premis, i}});
        root = root->children.first;
    }
    std::vector<Step> pro = {};
    int i = 0;
}
