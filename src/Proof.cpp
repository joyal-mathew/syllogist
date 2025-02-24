#include "Proof.hpp"
#include "TruthTree.hpp"


//--------------------------------//
//----- Forward Declarations -----//
//--------------------------------//

void dnR(TruthNode *curr, std::vector<Step> *proof);
void conR(TruthNode *curr, std::vector<Step> *proof);
void nconR(TruthNode *curr, std::vector<Step> *proof);
void disR(TruthNode *curr, std::vector<Step> *proof);
void ndisR(TruthNode *curr, std::vector<Step> *proof);
void condR(TruthNode *curr, std::vector<Step> *proof);
void ncondR(TruthNode *curr, std::vector<Step> *proof);
void bcondR(TruthNode *curr, std::vector<Step> *proof);
void nbcondR(TruthNode *curr, std::vector<Step> *proof);
void closeR(TruthNode *curr, std::vector<Step> *proof);
void checkNode(TruthNode *curr, std::vector<Step> *proof);

//--------------------------------//
//------- Proof Functions --------//
//--------------------------------//


Proof to_proof(std::pair<TruthNode *, int> tt){
    TruthNode* root = tt.first;
    std::vector<Step> premis = {}
    for (int i = 0; i < tt.second; i++){
        
    }
    checkNode()
}