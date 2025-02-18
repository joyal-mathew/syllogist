#include "TruthTree.hpp"
#include "Expr.hpp"
#include "errors.hpp"
#include <list>
#include <iostream>

// Forward Declarations
bool decomposable(TruthNode *node);
void add_child(TruthNode *node, TruthNode *lhs, TruthNode *rhs);
std::vector<TruthNode *> get_leaves(TruthNode *node);
void closure_check(TruthNode *root, std::vector<Expr> seen);
bool is_valid(TruthNode *root);

// Checks if expression is decomposable
bool decomposable(TruthNode *node){
    DEBUG_ASSERT(node->expr.type != ExprType::Contradiction);
    if (node->expr.type == ExprType::Atom)
        return false;
    if (node->expr.type == ExprType::Negation) {
        Expr* unnegated = node->expr.get_unnegation();
        if (unnegated->type == ExprType::Atom){
            delete unnegated;
            return false;
        }
        delete unnegated;
        return true;
    }
    return true;
}

// Adds children to a node
void add_child(TruthNode *node, TruthNode *lhs, TruthNode *rhs){
    node->children.first = lhs;
    node->children.second = rhs;
}

// Gets the leaves of a node
std::vector<TruthNode *> get_leaves(TruthNode *node) {
    node = node;
    return std::vector<TruthNode *>();
}

// Check for closures on branches
void closure_check(TruthNode *root, std::vector<Expr> seen) {
    root = root;
    seen = seen;
}

// Check all branches end in a closure
bool is_valid(TruthNode *root) {
    root = root;
    return true;
}

void delete_truth_tree(TruthNode *root) {
    if (root->children.first != NULL)
        delete_truth_tree(root->children.first);
    if (root->children.second != NULL)
        delete_truth_tree(root->children.second);
    delete root;
}

std::pair<TruthNode *, int> compute_truth_tree(std::vector<Expr *> premises){
    std::list<TruthNode *> unexpanded;
    TruthNode *root = new TruthNode(Expr(*premises[0]), DecompositionRule::Assume);
    if (decomposable(root))
        unexpanded.push_back(root);

    // Add Remaining Premises
    u64 i = 1;
    TruthNode *last_node = root;
    while (i < premises.size()){
        TruthNode* child = new TruthNode(Expr(*premises[i]), DecompositionRule::Assume);
        if (decomposable(child))
        unexpanded.push_back(child);
        add_child(last_node, child, nullptr);
        last_node = child;
        i++;
    }

    return std::pair<TruthNode *, int>{root, premises.size()};
    
    // Check list in order of stacks then branches
    // decompose first stack, if none then first branch
    // call get leaves to then create new nodes at the leaves and add to list, remove current from list
    // check for closures
    // repeat above block until undecomposed is empty
    // Check for open branches
}