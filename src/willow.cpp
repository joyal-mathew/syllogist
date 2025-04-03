#include <fstream>
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
    (void) root;
    (void) premise_count;

    std::unordered_map<TruthNode*, int> NodeId;
    std::vector<node> nodes;

    // Traverse the tree creating a json object out of it
        // First path constructs the object with ids, children and stuff
        // Second pass for decomposition and antecedent

    int cur_id = 0;
    TruthNode* cur_node = root;
    for (int i = 0; i < premise_count; i++) {
        nodes.push_back(node(cur_id, "tmp", true, cur_id-1));
        cur_id++;
        if (i != premise_count-1)
            nodes[i].children = std::vector<int>{cur_id};
        cur_node = cur_node->children.first;
    }

    // BFS traversal of tree to initially construct nodes

    // Json object for willow file
    json j;

    // Add nodes to json object
    j["nodes"] = nodes;
    
    // Willow Options
    j["options"] = {{"requireAtomicContradiction", true},{"requireAllBranchesTerminated", true, },{"lockedOptions", false}};
    
    // Write json file
    std::ofstream file;
    file.open("build/willow.willow");
    file << j;
    file.close();
}