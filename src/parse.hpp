#pragma once

#include "Expr.hpp"
#include "Proof.hpp"

#include <string>
#include <vector>

class ParseContext {
private:
    u8 index_table[256];
    std::vector<u8> symbol_table;

public:
        Expr root;
};

ParseContext parse(const std::string &string);
void render_proof(const Proof &proof);
