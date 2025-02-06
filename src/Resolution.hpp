#pragma once

#include "Clause.hpp"

#include <utility>
#include <vector>

struct Node {
    u16 clause_handle;
    std::pair<Node *, Node *> parents;
    u8 resolution_index;
    bool is_singleton;
};

struct ResolutionContext {
    std::vector<Clause> clauses;
    Node root;
};

ResolutionContext resolution(std::vector<Clause> argument);
