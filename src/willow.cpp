#include <fstream>
#include <queue>
#include <unordered_map>
#include "json.hpp"
#include "willow.hpp"

using json = nlohmann::json;

void to_json(json& j, const node& node) {
    j = json {
        {"id", node.id},
        {"text", node.text},
        {"children", node.children},
        {"decomposition", node.decomposition},
        {"premise", node.premise},
        {"parent", (node.parent == -1) ? json(nullptr) : json(node.parent)},
        {"antecedent", (node.antecedent == -1) ? json(nullptr) : json(node.antecedent)}
    };
}

void to_willow(TruthNode* root, int premise_count) {
    std::unordered_map<TruthNode*, int> node_map;
    std::vector<node> nodes;

    // Traverse the tree creating a json object out of it
        // First path constructs the object with ids, children and stuff
        // Second pass for decomposition and antecedent

    int cur_id = 0;
    TruthNode* cur_node = root;
    for (int i = 0; i < premise_count; i++) {
        nodes.push_back(node(cur_id, cur_node->expr.to_string(true), true, cur_id-1, -1));
        node_map.emplace(cur_node, cur_id);
        cur_id++;
        if (i != premise_count - 1)
            cur_node = cur_node->children.first;
    }

    // BFS to construct nodes and add them to map
    std::queue<TruthNode*> q;
    if (cur_node->children.first)
        q.push(cur_node->children.first);
    if (cur_node->children.second)
        q.push(cur_node->children.second);
    while (!q.empty()) {
        cur_node = q.front();
        q.pop();
        if (cur_node->children.first)
            q.push(cur_node->children.first);
        if (cur_node->children.second)
            q.push(cur_node->children.second);
        nodes.push_back(node(cur_id, cur_node->expr.to_string(true), false));
        node_map.emplace(cur_node, cur_id);
        cur_id++;
    }

    // BFS to get children, antecedents
    q.push(root);
    while (!q.empty()) {
        TruthNode* cur_node = q.front();
        q.pop();
        if (cur_node->children.first) {
            q.push(cur_node->children.first);
            nodes[node_map.at(cur_node)].children.push_back(node_map.at(cur_node->children.first));
            nodes[node_map.at(cur_node->children.first)].parent = node_map.at(cur_node);
        }
        if (cur_node->children.second) {
            q.push(cur_node->children.second);
            nodes[node_map.at(cur_node)].children.push_back(node_map.at(cur_node->children.second));
            nodes[node_map.at(cur_node->children.second)].parent = node_map.at(cur_node);
        }
        if (cur_node->references.first) {
            nodes[node_map.at(cur_node)].antecedent = node_map.at(cur_node->references.first);
            nodes[node_map.at(cur_node->references.first)].decomposition.push_back(node_map.at(cur_node));
        }
        if (cur_node->references.second) { // If a node has 2 references it is a closure
            // Revert reference.first changes then add both references to decomposition
            nodes[node_map.at(cur_node)].antecedent = -1;
            nodes[node_map.at(cur_node->references.first)].decomposition.pop_back();
            nodes[node_map.at(cur_node)].decomposition.push_back(node_map.at(cur_node->references.first));
            nodes[node_map.at(cur_node)].decomposition.push_back(node_map.at(cur_node->references.second));
        }
    }

    // Json object for willow file
    json j;

    // Add nodes to json object
    j["nodes"] = nodes;
    
    // Willow Options
    j["options"] = {{"requireAtomicContradiction", true},{"requireAllBranchesTerminated", true, },{"lockedOptions", false}};
    
    // Write json file
    std::ofstream file;
    file.open("build/willow.willow");
    file << j.dump(2);
    file.close();
}