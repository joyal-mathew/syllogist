#include "TruthTree.hpp"
#include "Expr.hpp"
#include "errors.hpp"
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

// Forward Declarations
bool decomposable(TruthNode *node);
void add_child(TruthNode *node, TruthNode *lhs, TruthNode *rhs);
std::vector<TruthNode *> get_leaves(TruthNode *node);
void closure_check(TruthNode *root, std::vector<Expr> seen);
bool is_valid(TruthNode *root);
std::pair<TruthNode *, DecompositionRule::DecompositionRule> get_next_decomposition(std::list<TruthNode *> &list);
std::pair<TruthNode *, TruthNode *> get_decomposition_children(TruthNode *node, DecompositionRule::DecompositionRule rule);

// Checks if expression is decomposable
bool decomposable(TruthNode *node) {
    if (node == nullptr)
        return false;
    DEBUG_ASSERT(node->expr.type != ExprType::Contradiction);
    if (node->expr.type == ExprType::Atom)
        return false;
    if (node->expr.type == ExprType::Negation) {
        ExprType::ExprType unnegated_type = node->expr.get_unnegation_type(); 
        return (unnegated_type == ExprType::Atom) ? false : true;
    }
    return true;
}

// Adds children to a node
void add_child(TruthNode *node, TruthNode *lhs, TruthNode *rhs) {
    node->children.first = lhs;
    node->children.second = rhs;
}

// Gets the leaves of a node
std::vector<TruthNode *> get_leaves(TruthNode *node) {
    if (node == nullptr)
        return std::vector<TruthNode *>();
    if (node->children.first == nullptr && node->children.second == nullptr) {
        if (node->expr.type == ExprType::Contradiction)
            return std::vector<TruthNode *>();
        return std::vector<TruthNode *>{node};
    }
    std::vector<TruthNode *> lhs = get_leaves(node->children.first);
    std::vector<TruthNode *> rhs = get_leaves(node->children.second);
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
}

// Check for closures on branches
void closure_check(TruthNode *root, std::vector<Expr> seen) {
    root = root;
    seen = seen;
}

// Check all branches end in a closure
bool is_valid(TruthNode *root) {
    std::vector<TruthNode *> leaves = get_leaves(root);
    for (uint i = 0; i < leaves.size(); i++) {
        if (leaves[i]->expr.type != ExprType::Contradiction)
            return false;
    }
    return true;
}

// Returns next decomposition node and rule. Also removes from list.
std::pair<TruthNode *, DecompositionRule::DecompositionRule> get_next_decomposition(std::list<TruthNode *> &list){
    TruthNode *node = nullptr;
    DecompositionRule::DecompositionRule rule = DecompositionRule::Assume; // Assume is used as a null
    std::list<TruthNode *>::iterator iter = list.begin();

    while (iter != list.end()) { // Check if any stacking cases
        node = *iter;
        if (node->expr.type == ExprType::Negation){
            ExprType::ExprType unnegated_type = node->expr.get_unnegation_type();
            switch (unnegated_type) {
                case ExprType::Negation:
                    rule = DecompositionRule::DoubleNegation;
                    break;
                case ExprType::Disjunction:
                    rule = DecompositionRule::NegatedDisjunction;
                    break;
                case ExprType::Conditional:
                    rule = DecompositionRule::NegatedConditional;
                    break;
                default:
                    break;
            }
            if (rule != DecompositionRule::Assume) {
                list.erase(iter);
                return std::pair<TruthNode *, DecompositionRule::DecompositionRule>{node, rule};
            }
        }
        else if (node->expr.type == ExprType::Conjunction){
            rule = DecompositionRule::Conjunction;
            list.erase(iter);
            return std::pair<TruthNode *, DecompositionRule::DecompositionRule>{node, rule}; 
        }
        iter++;
    }

    // The remaining branch cases
    node = list.front();
    ExprType::ExprType expr_type;
    if (node->expr.type == ExprType::Negation) {
        expr_type = node->expr.get_unnegation_type();
        switch (expr_type) {
            case ExprType::Conjunction:
                rule = DecompositionRule::NegatedConjunction;
                break;
            case ExprType::Biconditional:
                rule = DecompositionRule::NegatedBiconditional;
                break; 
            default:
                break;
        }
    } else {
        expr_type = node->expr.type; 
        switch (expr_type) {
            case ExprType::Disjunction:
                rule = DecompositionRule::Disjunction;
                break;
            case ExprType::Conditional:
                rule = DecompositionRule::Conditional;
                break;
            case ExprType::Biconditional:
                rule = DecompositionRule::Biconditional;
                break; 
            default:
                break;
        }
    }
    DEBUG_ASSERT(node != nullptr);
    DEBUG_ASSERT(rule != DecompositionRule::Assume);
    list.pop_front();
    return std::pair<TruthNode *, DecompositionRule::DecompositionRule>{node, rule};
}

//Returns the children of a node and decomposition rule applied to it.
std::pair<TruthNode *, TruthNode *> get_decomposition_children(TruthNode *node, DecompositionRule::DecompositionRule rule) {
    //! Good chance this function causes a bunch of memory leaks
    TruthNode *lhs = nullptr;
    TruthNode *rhs = nullptr;
    switch (rule) {
        case DecompositionRule::DoubleNegation: {
            Expr *tmp = node->expr.get_unnegation();
            tmp->unnegate();
            lhs = new TruthNode(Expr(*tmp), rule, node);
            delete tmp;
            break; }
        case DecompositionRule::Conjunction: {
            std::pair<Expr, Expr> decomposed = node->expr.decompose();
            lhs = new TruthNode(decomposed.first, rule, node);
            TruthNode *lhs2 = new TruthNode(decomposed.second, rule, node);
            add_child(lhs, lhs2, nullptr);
            break; }
        case DecompositionRule::NegatedConjunction: {
            Expr *unnegated = node->expr.get_unnegation();
            std::pair<Expr, Expr> decomposed = unnegated->decompose();
            decomposed.first.negate();
            decomposed.second.negate();
            lhs = new TruthNode(decomposed.first, rule, node);
            rhs = new TruthNode(decomposed.second, rule, node);
            delete unnegated;
            break; }
        case DecompositionRule::Disjunction: {
            std::pair<Expr, Expr> decomposed = node->expr.decompose();
            lhs = new TruthNode(decomposed.first, rule, node);
            rhs = new TruthNode(decomposed.second, rule, node);
            break; }
        case DecompositionRule::NegatedDisjunction: {
            Expr *unnegated = node->expr.get_unnegation();
            std::pair<Expr, Expr> decomposed = unnegated->decompose();
            decomposed.first.negate();
            decomposed.second.negate();
            lhs = new TruthNode(decomposed.first, rule, node);
            TruthNode *lhs2 = new TruthNode(decomposed.second, rule, node);
            add_child(lhs, lhs2, nullptr);
            delete unnegated;
            break; }
        case DecompositionRule::Conditional: {
            std::pair<Expr, Expr> decomposed = node->expr.decompose();
            decomposed.first.negate();
            lhs = new TruthNode(decomposed.first, rule, node);
            rhs = new TruthNode(decomposed.second, rule, node);
            break; }
        case DecompositionRule::NegatedConditional: {
            Expr *unnegated = node->expr.get_unnegation();
            std::pair<Expr, Expr> decomposed = unnegated->decompose();
            decomposed.second.negate();
            lhs = new TruthNode(decomposed.first, rule, node);
            TruthNode *lhs2 = new TruthNode(decomposed.second, rule, node);
            add_child(lhs, lhs2, nullptr);
            delete unnegated;
            break; }
        case DecompositionRule::Biconditional: {
            std::pair<Expr, Expr> decomposed = node->expr.decompose();
            Expr *negated_lhs = decomposed.first.get_negation();
            Expr *negated_rhs = decomposed.second.get_negation();
            lhs = new TruthNode(decomposed.first, rule, node);
            TruthNode *lhs2 = new TruthNode(decomposed.second, rule, node);
            add_child(lhs, lhs2, nullptr);
            rhs = new TruthNode(*negated_lhs, rule, node);
            TruthNode *rhs2 = new TruthNode(*negated_rhs, rule, node);
            add_child(rhs, rhs2, nullptr);
            delete negated_lhs;
            delete negated_rhs;
            break; }
        case DecompositionRule::NegatedBiconditional: {
            Expr *unnegated = node->expr.get_unnegation();
            std::pair<Expr, Expr> decomposed = unnegated->decompose();
            Expr *negated_lhs = decomposed.first.get_negation();
            Expr *negated_rhs = decomposed.second.get_negation();
            lhs = new TruthNode(decomposed.first, rule, node);
            TruthNode *lhs2 = new TruthNode(*negated_rhs, rule, node);
            add_child(lhs, lhs2, nullptr);
            rhs = new TruthNode(*negated_lhs, rule, node);
            TruthNode *rhs2 = new TruthNode(decomposed.second, rule, node);
            add_child(rhs, rhs2, nullptr);
            delete unnegated;
            delete negated_lhs;
            delete negated_rhs;
            break; }
        default:
            break;
    }
    DEBUG_ASSERT(lhs != nullptr);
    return std::pair<TruthNode *, TruthNode *>{lhs, rhs};
}

void delete_truth_tree(TruthNode *root) {
    if (root == nullptr)
        return;
    delete_truth_tree(root->children.first);
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
        TruthNode *child = new TruthNode(Expr(*premises[i]), DecompositionRule::Assume);
        if (decomposable(child))
            unexpanded.push_back(child);
        add_child(last_node, child, nullptr);
        last_node = child;
        i++;
    }

    while (!unexpanded.empty()){
        std::pair<TruthNode *, DecompositionRule::DecompositionRule> decompose = get_next_decomposition(unexpanded);
        std::pair<TruthNode *, TruthNode *> decomposed_children = get_decomposition_children(decompose.first, decompose.second);
        std::vector<TruthNode *> leaves = get_leaves(decompose.first);
        for (uint i = 0; i < leaves.size(); i++) {
            TruthNode *first_child_copy = (decomposed_children.first) ? new TruthNode(*decomposed_children.first) : nullptr;
            TruthNode *second_child_copy = (decomposed_children.second) ? new TruthNode(*decomposed_children.second) : nullptr;
            add_child(leaves[i], first_child_copy, second_child_copy);
            if (first_child_copy != nullptr) {
                if (decomposable(first_child_copy))
                    unexpanded.push_back(first_child_copy);
                if (decomposable(first_child_copy->children.first))
                    unexpanded.push_back(first_child_copy->children.first);
            }
            if (second_child_copy != nullptr) {
                if (decomposable(second_child_copy))
                    unexpanded.push_back(second_child_copy);
                if (decomposable(second_child_copy->children.first))
                    unexpanded.push_back(second_child_copy->children.first);
            }
        }
        delete_truth_tree(decomposed_children.first); // Also deletes children
        delete_truth_tree(decomposed_children.second);
    }

    return std::pair<TruthNode *, int>{root, premises.size()};
    
    // Check list in order of stacks then branches
    // decompose first stack, if none then first branch
    // call get leaves to then create new nodes at the leaves and add to list, remove current from list
    // check for closures
    // repeat above block until unexpanded is empty
    // Check for open branches
}

//--------------------------------//
//----- Exporting Truth Tree -----//
//--------------------------------//

void write_dot_node(std::ofstream &out, TruthNode *node, bool show_refs = false) {
    if (node == nullptr) return;
    std::stringstream label;
    label << node->expr;
    std::string label_str = label.str();
    size_t pos = 0;
    while ((pos = label_str.find('"', pos)) != std::string::npos) {
        label_str.insert(pos, "\\");
        pos += 2;
    }
    out << "    \"" << node << "\" [label=\"" << label_str << "\"];\n";
    if (node->children.first != nullptr) {
        out << "    \"" << node << "\" -> \"" << node->children.first << "\";\n";
        write_dot_node(out, node->children.first);
    }
    if (node->children.second != nullptr) {
        out << "    \"" << node << "\" -> \"" << node->children.second << "\";\n";
        write_dot_node(out, node->children.second);
    }
    //show_refs = true; // For debugging...
    if (show_refs){
        if (node->references.first != nullptr) {
            out << "    \"" << node << "\" -> \"" << node->references.first << "\" [style=\"dashed\"];\n";
        }
        if (node->references.second != nullptr) {
            out << "    \"" << node << "\" -> \"" << node->references.second << "\" [style=\"dashed\"];\n";
        }
    }
}

void export_truth_tree_to_dot(TruthNode *root) {
    const std::string filename = "build/truth_tree.dot";
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }
    out << "digraph TruthTree {\n";
    out << "    rankdir=TB;\n";
    out << "    node [shape=ellipse];\n";
    write_dot_node(out, root); 
    out << "}\n";
    out.close();
}