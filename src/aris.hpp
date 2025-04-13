#pragma once

#include <stdlib.h>
#include "Proof.hpp"

void subproof(std::ostream& file, std::vector<Step>& proof, int& line_num, int& indent_level, int& proof_num);

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