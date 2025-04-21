#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <codecvt>
#include <locale>
#include "aris.hpp"
#include "Expr.hpp"
#include "Proof.hpp"

// Aligns the rules (thank god for ai)
void align(const std::string& input_file, const std::string& output_file, int max_line_number) {
    std::ifstream in_file(input_file);
    std::vector<std::string> lines;
    std::vector<size_t> colon_positions;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string line;
    size_t max_display_width = 0;

    size_t max_line_num_width = std::to_string(max_line_number).length();

    while (std::getline(in_file, line)) {
        // Split line number and the rest
        size_t dot_pos = line.find('.');
        if (dot_pos != std::string::npos) {
            std::string num_part = line.substr(0, dot_pos);
            std::string rest = line.substr(dot_pos + 1);

            // Trim leading space after dot
            if (!rest.empty() && rest[0] == ' ') rest = rest.substr(1);

            // Pad to align with max width
            size_t padding_needed = max_line_num_width - num_part.length();
            line = num_part + "." + std::string(padding_needed + 1, ' ') + rest;
        }

        lines.push_back(line);

        size_t byte_pos = line.find(" : ");
        if (byte_pos != std::string::npos) {
            std::wstring wide_prefix = converter.from_bytes(line.substr(0, byte_pos));
            size_t display_width = wide_prefix.length();
            colon_positions.push_back(display_width);
            max_display_width = std::max(max_display_width, display_width);
        } else {
            colon_positions.push_back(static_cast<size_t>(-1));
        }
    }

    in_file.close();

    std::ofstream out_file(output_file);
    for (size_t i = 0; i < lines.size(); ++i) {
        if (colon_positions[i] != static_cast<size_t>(-1)) {
            size_t byte_pos = lines[i].find(" : ");
            std::string prefix = lines[i].substr(0, byte_pos);
            std::string suffix = lines[i].substr(byte_pos);
            size_t padding = max_display_width - colon_positions[i];
            out_file << prefix << std::string(padding, ' ') << suffix << "\n";
        } else {
            out_file << lines[i] << "\n";
        }
    }

    out_file.close();
}

std::string sub_proof_indent(int length) {
    std::string res;
    for (int i = 0; i < length; i++) {
        res += "| ";
    }
    return res;
}

std::string rule_to_string(InferenceRule::InferenceRule rule) {
    switch (rule) {
        case InferenceRule::ContradictionIntroduction: return "CONTRADICTION";
        case InferenceRule::ContradictionElimination: return "PRINCIPLE_OF_EXPLOSION";
        case InferenceRule::NegationIntroduction: return "PROOF_BY_CONTRADICTION";
        case InferenceRule::NegationElimination: return "DOUBLENEGATION";
        case InferenceRule::ConjunctionIntroduction: return "CONJUNCTION";
        case InferenceRule::ConjunctionElimination: return "SIMPLIFICATION";
        case InferenceRule::DisjunctionIntroduction: return "ADDITION";
        case InferenceRule::DisjunctionElimination: return "DISJUNCTIVE_ELIMINATION";
        case InferenceRule::ConditionalIntroduction: return "CONDITIONAL_PROOF";
        case InferenceRule::ConditionalElimination: return "MODUS_PONENS";
        case InferenceRule::BiconditionalIntroduction: return "BICONDITIONAL_INTRO";
        case InferenceRule::BiconditionalElimination: return "BICONDITIONAL_ELIM";
        default: return "EMPTY_RULE"; // None for assumption or reiteration
    }
}

std::string rule_to_string_plain(InferenceRule::InferenceRule rule) {
    switch (rule) {
        case InferenceRule::Assume: return "Assume";
        case InferenceRule::Reiteration: return "Reit";
        case InferenceRule::ContradictionIntroduction: return "⊥ Intro";
        case InferenceRule::ContradictionElimination: return "⊥ Elim";
        case InferenceRule::NegationIntroduction: return "¬ Intro";
        case InferenceRule::NegationElimination: return "¬ Elim";
        case InferenceRule::ConjunctionIntroduction: return "∧ Intro";
        case InferenceRule::ConjunctionElimination: return "∧ Elim";
        case InferenceRule::DisjunctionIntroduction: return "∨ Intro";
        case InferenceRule::DisjunctionElimination: return "∨ Elim";
        case InferenceRule::ConditionalIntroduction: return "→ Intro";
        case InferenceRule::ConditionalElimination: return "→ Elim";
        case InferenceRule::BiconditionalIntroduction: return "↔ Intro";
        case InferenceRule::BiconditionalElimination: return "↔ Elim";
        default: return "Unknown Rule";
    }
}

std::string reference_print(std::vector<StepLoc>& refs, InferenceRule::InferenceRule rule) {
    (void) rule;
    std::string res = " (";
    for (unsigned int i = 0; i < refs.size(); i++) {
        Step* step = &refs[i].first->at(refs[i].second);
        res += std::to_string(step->line_number);
        if (step->has_subproof()) {
            switch (rule) {
                case InferenceRule::DisjunctionElimination:
                case InferenceRule::NegationIntroduction:
                case InferenceRule::ConditionalIntroduction:
                case InferenceRule::BiconditionalIntroduction: {
                    res+= "-" + std::to_string(step->subproof.at(step->subproof.size()-1).line_number);
                    break; }
                
                default:
                    break;
            }
        }
        if (i != refs.size()-1)
            res+=", ";
    }
    res += ")";
    return res;
}

void step_print(std::ofstream& file, Step &step) {
    file << "    <step linenum=\"" << step.line_number << "\">\n";
    file << "      <raw>" << step.expr.to_string() << "</raw>\n";
    file << "      <rule>" << rule_to_string(step.rule) << "</rule>\n";
    for (unsigned int i = 0; i < step.references.value().size(); i++) {
        Step* ref_step = &step.references.value()[i].first->at(step.references.value()[i].second);
        file << "      <premise>" << ref_step->line_number << "</premise>\n";
    }
    file << "    </step>\n";
}

void proof_print(std::ofstream& file, std::vector<Step> &proof, Step* assumption, std::vector<std::pair<std::vector<Step>*, Step*>> &proof_vec) {
    if (assumption != nullptr) { // Only ever false for root proof (proofs[0])
        file << "    <assumption linenum=\"" << assumption->line_number << "\">\n";
        file << "       <raw>" << assumption->expr.to_string() << "</raw>\n";
        file << "    </assumption>\n";
    }
    for (unsigned int i = 0; i < proof.size(); i++) {
        if (proof[i].has_subproof()) {
            file << "    <step linenum=\"" << proof[i].line_number << "\">\n";
            file << "      <rule>SUBPROOF</rule>\n";
            file << "      <premise>" << proof_vec.size() << "</premise>\n";
            file << "    </step>\n";
            proof_vec.push_back(std::make_pair(&proof[i].subproof, &proof[i]));
        } else {
            step_print(file, proof[i]);
        }
    }
}

void to_aris(Proof& proof) {
    std::vector<std::pair<std::vector<Step>*, Step*>> proofs; // First is subproof, Second is its assumption
    std::ofstream file;
    file.open("build/aris.txt");

    // Header
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    file << "<bram>\n";
    file << "  <program>Aris</program>\n";
    file << "  <version>0.1.0</version>\n";
    file << "  <proof id=\"" << proofs.size() <<"\">\n";
    
    // Premises
    for (unsigned int i = 0; i < proof.premises.size(); i++) {
        file << "    <assumption linenum=\"" << proof.premises[i].line_number << "\">\n"; // Run plain proof first for this to not break
        file << "      <raw>" << proof.premises[i].expr.to_string() << "</raw>\n";
        file << "    </assumption>\n";
    }

    proofs.push_back(std::make_pair(&proof.proof, nullptr));

    // Proof
    for (unsigned int i = 0; i < proofs.size(); i++) {
        if (i != 0)
            file << "  <proof id=\"" << i << "\">\n";
        proof_print(file, *proofs[i].first, proofs[i].second, proofs);
        file << "  </proof>\n";
    }

    file << "</bram>";
    file.close();
}

void to_plain_subproof(std::ofstream& file, std::vector<Step>& proof, int& line_num, int& indent_level) {
    std::string subproof_end;
    for (unsigned int i = 0; i < proof.size(); i++) {
        if (proof[i].has_subproof()) { // Subproof
            file << line_num << ". " << sub_proof_indent(indent_level) << "┌ " << proof[i].expr.to_string() << " : " << rule_to_string_plain(proof[i].rule) << "\n";
            proof[i].line_number = line_num;
            line_num++;
            to_plain_subproof(file, proof[i].subproof, line_num, ++indent_level);
        } else { // Regular Step
            if (indent_level > 0 && (i+1 == proof.size())) {
                subproof_end = "└ ";
                indent_level--;
            }
            file << line_num << ". " << sub_proof_indent(indent_level) << subproof_end << proof[i].expr.to_string() << " : " << rule_to_string_plain(proof[i].rule) << reference_print(proof[i].references.value(), proof[i].rule) << "\n";
            // file << line_num << ". " << sub_proof_indent(indent_level) << subproof_end << proof[i].expr.to_string() << " : " << rule_to_string_plain(proof[i].rule) << "\n";
            proof[i].line_number = line_num;
            line_num++;
        }
    }
}

void to_plain(Proof& proof) {
    int indent_level = 0;
    int line_num = 0;
    std::string indent(indent_level * 2, ' ');
    std::ofstream file;
    file.open("build/proof.txt");

    // Premises
    for (unsigned int i = 0; i < proof.premises.size(); i++) {
        file << line_num << ". " << indent << proof.premises[i].expr.to_string() << " : " << "Premise" << "\n";
        proof.premises[i].line_number = line_num;
        line_num++;
    }
    to_plain_subproof(file, proof.proof, line_num, indent_level);

    file.close();

    align("build/proof.txt", "build/proof_aligned.txt", proof.proof[proof.proof.size()-1].line_number);
}