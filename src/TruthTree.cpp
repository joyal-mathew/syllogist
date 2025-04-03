#include "TruthTree.hpp"
#include "Expr.hpp"
#include "errors.hpp"
#include "willow.hpp"
#include <list>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>

//--------------------------------//
//----- Forward Declarations -----//
//--------------------------------//

bool decomposable(TruthNode *node);
void add_child(TruthNode *node, TruthNode *lhs, TruthNode *rhs);
std::vector<TruthNode *> get_leaves(TruthNode *node);
void closure_check(TruthNode *root, std::vector<Expr> seen);
bool is_valid(TruthNode *root);
std::pair<TruthNode *, DecompositionRule::DecompositionRule> get_next_decomposition(std::list<TruthNode *> &list);
std::pair<TruthNode *, TruthNode *> get_decomposition_children(TruthNode *node, DecompositionRule::DecompositionRule rule);

//--------------------------------//
//----- Truth Tree Functions -----//
//--------------------------------//

/**
 * @brief Checks if a node is decomposable
 * @param node The node
 * @return true If node is decomposable
 * @return false If node is not decomposable
 */
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

/**
 * @brief Adds a left and right child to a parent node
 * @param node The node
 * @param lhs Left child
 * @param rhs Right child
 */
void add_child(TruthNode *node, TruthNode *lhs, TruthNode *rhs) {
    node->children.first = lhs;
    node->children.second = rhs;
}

/**
 * @brief Gets the leaves of a node
 * @param node The starting node
 * @return std::vector<TruthNode *> 
 */
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

/**
 * @brief Check for closures on branches
 * @param root The root of the truth tree
 * @param seen List of seen atoms
 */
void closure_check(TruthNode *root, std::unordered_map<u16, TruthNode*> seen = std::unordered_map<u16, TruthNode*>()) {
    if (root == nullptr)
        return;
    bool isAtom = false;
    bool isNegated = false;
    switch (root->expr.type) {
        case ExprType::Atom:
            isAtom = true;
            break;
        case ExprType::Negation:
            isAtom = (root->expr.get_unnegation_type() == ExprType::Atom);
            isNegated = true;
            break;
    default:
        break;
    }
    if (isAtom) {
        u16 atom = root->expr.get_atom();
        if (isNegated)
            atom *= -1;
        if (seen.count(atom * -1)) {
            delete_truth_tree(root->children.first);
            delete_truth_tree(root->children.second);
            TruthNode *con = new TruthNode(Expr(ExprType::Contradiction), DecompositionRule::Closure);
            con->references = {seen.at(atom * -1), root};
            add_child(root, con, nullptr);
            return;
        }
        seen.emplace(atom, root);
    }
    closure_check(root->children.first, seen);
    closure_check(root->children.second, seen);
}

/**
 * @brief Checks that all branches end in a closure
 * @param root Root of the truth tree
 * @return true When all branches end with a contradiction
 * @return false When there exist at least one open branch
 */
bool is_valid(TruthNode *root) {
    std::vector<TruthNode *> leaves = get_leaves(root);
    for (uint i = 0; i < leaves.size(); i++) {
        if (leaves[i]->expr.type != ExprType::Contradiction)
            return false;
    }
    return true;
}

/**
 * @brief Returns next decomposition node and rule from unexpanded node list.
 * @param list List of unexpanded truth nodes (modified)
 * @return std::pair<TruthNode *, DecompositionRule::DecompositionRule> 
 */
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

/**
 * @brief Returns the children of a node and decomposition rule applied to it.
 * @param node The node being decomposed
 * @param rule The decomposition rule
 * @return std::pair<TruthNode *, TruthNode *> 
 */
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
            Expr *first_copy = new Expr(decomposed.first);
            Expr *second_copy = new Expr(decomposed.second);
            Expr *negated_lhs = decomposed.first.get_negation();
            Expr *negated_rhs = decomposed.second.get_negation();
            lhs = new TruthNode(Expr(ExprType::Conjunction, first_copy, second_copy), rule, node);
            rhs = new TruthNode(Expr(ExprType::Conjunction, negated_lhs, negated_rhs), rule, node);
            break; }
        case DecompositionRule::NegatedBiconditional: {
            Expr *unnegated = node->expr.get_unnegation();
            std::pair<Expr, Expr> decomposed = unnegated->decompose();
            Expr *first_copy = new Expr(decomposed.first);
            Expr *second_copy = new Expr(decomposed.second);
            Expr *negated_lhs = decomposed.first.get_negation();
            Expr *negated_rhs = decomposed.second.get_negation();
            lhs = new TruthNode(Expr(ExprType::Conjunction, first_copy, negated_rhs), rule, node);
            rhs = new TruthNode(Expr(ExprType::Conjunction, negated_lhs, second_copy), rule, node);
            delete unnegated;
            break; }
        default:
            break;
    }
    DEBUG_ASSERT(lhs != nullptr);
    return std::pair<TruthNode *, TruthNode *>{lhs, rhs};
}

/**
 * @brief Deletes a truth tree
 * @param root The starting node
 */
void delete_truth_tree(TruthNode *root) {
    if (root == nullptr)
        return;
    delete_truth_tree(root->children.first);
    delete_truth_tree(root->children.second);
    delete root;
}

/**
 * @brief Creates a truth tree from a list of premises
 * @param premises List of premises
 * @return std::pair<TruthNode *, int> 
 */
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

    // Handle all unexpanded nodes
    while (!unexpanded.empty()){
        std::pair<TruthNode *, DecompositionRule::DecompositionRule> decompose = get_next_decomposition(unexpanded);
        std::vector<TruthNode *> leaves = get_leaves(decompose.first);
        for (uint i = 0; i < leaves.size(); i++) {
            std::pair<TruthNode *, TruthNode *> decomposed_children = get_decomposition_children(decompose.first, decompose.second);
            add_child(leaves[i], decomposed_children.first, decomposed_children.second);
            if (decomposed_children.first != nullptr) {
                if (decomposable(decomposed_children.first))
                    unexpanded.push_back(decomposed_children.first);
                if (decomposable(decomposed_children.first->children.first))
                    unexpanded.push_back(decomposed_children.first->children.first);
            }
            if (decomposed_children.second != nullptr) {
                if (decomposable(decomposed_children.second))
                    unexpanded.push_back(decomposed_children.second);
                if (decomposable(decomposed_children.second->children.first))
                    unexpanded.push_back(decomposed_children.second->children.first);
            }
        }
    }
    closure_check(root);

    export_truth_tree_to_dot(root);

    if (!is_valid(root)){
        to_willow(root, premises.size());
        delete_truth_tree(root);
        return std::pair<TruthNode *, int>{nullptr, 0};
    }

    return std::pair<TruthNode *, int>{root, premises.size()};
}

//--------------------------------//
//----- Exporting Truth Tree -----//
//--------------------------------//

/**
 * @brief Writes a truth tree node as a vertex and calls its children as edges
 * @param out The file being written to
 * @param node The truth tree node
 * @param show_refs Optional parameter for displaying reference edges
 */
void write_dot_node(std::ofstream &out, TruthNode *node, bool show_refs) {
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
        write_dot_node(out, node->children.first, show_refs);
    }
    if (node->children.second != nullptr) {
        out << "    \"" << node << "\" -> \"" << node->children.second << "\";\n";
        write_dot_node(out, node->children.second, show_refs);
    }
    if(show_refs){
        if (node->references.first != nullptr) {
            out << "    \"" << node << "\" -> \"" << node->references.first << "\" [style=\"dashed\"];\n";
        }
        if (node->references.second != nullptr) {
            out << "    \"" << node << "\" -> \"" << node->references.second << "\" [style=\"dashed\"];\n";
        }
    }
}

/**
 * @brief Write a truth tree as a dot file, written to build/truth_tree.dot
 * @param root The root of the truth tree
 * @param show_refs Optional parameter for displaying reference edges
 */
void export_truth_tree_to_dot(TruthNode *root, bool show_refs) {
    const std::string filename = "build/truth_tree.dot";
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }
    out << "digraph TruthTree {\n";
    out << "    rankdir=TB;\n";
    out << "    node [shape=ellipse];\n";
    write_dot_node(out, root, show_refs); 
    out << "}\n";
    out.close();
}