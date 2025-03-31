#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"
#include "TruthTree.hpp"

using json = nlohmann::json;

struct node {
    int id;
    std::string text;
    std::vector<int> children;
    std::vector<int> decomposition;
    bool premise;
    int antecedent;
    int parent;

    node(int id_, std::string text_, bool premise_, int parent_)
        : id(id_), text(text_), premise(premise_), parent(parent_) {}
};

/**
 * @brief Define serialization from nodes to json
 * @param json The json object
 * @param node The node
 */
void to_json(json& json, const node& node);

/**
 * @brief Converts a truth tree to a willow file
 * @param root The root of the truth tree
 * @param premise_count The number of premises
 */
void to_willow(TruthNode* root, int premise_count);
