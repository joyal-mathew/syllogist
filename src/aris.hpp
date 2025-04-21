#pragma once

#include <stdlib.h>
#include "Proof.hpp"


void proof_print(std::ofstream& file, std::vector<Step> &proof, Step* assumption, std::vector<std::pair<std::vector<Step>*, Step*>> &proof_vec);

void to_aris(Proof& proof);

void to_plain_subproof(std::ofstream& file, std::vector<Step>& proof, int& line_num, int& indent_level);

void to_plain(Proof& proof);

//* Aris Symbols
// ⊥
// ¬a
// (a ∧ a)
// (a ∨ a)
// (a ↔ a)
// (a → a)