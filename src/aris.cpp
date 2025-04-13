#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "aris.hpp"
#include "Expr.hpp"
#include "Proof.hpp"

std::string sub_proof_indent(int length) {
    std::string res;
    for (int i = 0; i < length; i++) {
        res += "| ";
    }
    return res;
}

std::string rule_to_string_plain(InferenceRule::InferenceRule rule) {
    switch (rule) {
        case InferenceRule::Assume: return "Assume";
        case InferenceRule::Reiteration: return "Reit.";
        case InferenceRule::ContradictionIntroduction: return "⊥ Intro.";
        case InferenceRule::ContradictionElimination: return "⊥ Elim.";
        case InferenceRule::NegationIntroduction: return "¬ Intro.";
        case InferenceRule::NegationElimination: return "¬ Elim.";
        case InferenceRule::ConjunctionIntroduction: return "∧ Intro.";
        case InferenceRule::ConjunctionElimination: return "∧ Elim.";
        case InferenceRule::DisjunctionIntroduction: return "∨ Intro.";
        case InferenceRule::DisjunctionElimination: return "∨ Elim.";
        case InferenceRule::ConditionalIntroduction: return "→ Intro.";
        case InferenceRule::ConditionalElimination: return "→ Elim.";
        case InferenceRule::BiconditionalIntroduction: return "↔ Intro.";
        case InferenceRule::BiconditionalElimination: return "↔ Elim.";
        default: return "Unknown Rule";
    }
}

std::string reference_print(std::vector<StepLoc>& refs) {
    std::string res;
    for (unsigned int i = 0; i < refs.size(); i++) {
        //res += " " + std::to_string(refs[i].first->at(refs[i].second).line_number);
    }
    return res;
}

void subproof(std::ostream& file, std::vector<Step>& proof, int& line_num, int& indent_level, int& proof_num) {
    std::string indent(indent_level * 2, ' ');
    for (unsigned int i = 0; i < proof.size() ; i++) {
        //file << indent << "<step linenum=\"" << line_num++ << "\">\n";
        file << indent <<  proof[i].expr.to_string() << "\n";
        //file << indent << "<rule>EMPTY_RULE</rule>" << "\n";
        //file << indent << "</step>\n";
        if (proof[i].subproof.has_value()) { // Start of new subproof
            subproof(file, proof[i].subproof.value(), line_num, ++indent_level, proof_num);
        }
    }
    indent_level--;
}

void to_aris(Proof& proof) {
    int indent_level = 1;
    int proof_num = 0;
    int line_num = 0;
    std::string indent(indent_level * 2, ' ');
    std::ofstream file;
    file.open("build/aris.txt");

    //! Switching to TinyXML soon...
    
    // Header
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    file << "<bram>\n";
    file << indent << "<program>Aris</program>\n";
    file << indent << "<version>0.1.0</version>\n";
    file << indent << "<proof id=\"" << proof_num <<"\">\n";
    proof_num++;
    
    // Premises
    for (unsigned int i = 0; i < proof.premises.size(); i++) {
        file << indent << "<assumption linenum=\"" << line_num++ << "\">\n";
        file << indent << "  <raw>" << proof.premises[i].expr.to_string() << "</raw>\n";
        file << indent << "</assumption>\n";
    }

    // Proof
    subproof(file, proof.proof, line_num, ++indent_level, ++proof_num);

    // Ender
    file << indent << "</proof>\n";
    file << "</bram>";
    

    file.close();
}

void to_plain_subproof(std::ofstream& file, std::vector<Step>& proof, int& line_num, int& indent_level) {
    std::string subproof_end;
    for (unsigned int i = 0; i < proof.size(); i++) {
        if (proof[i].subproof.has_value()) { // Subproof
            file << line_num++ << ". " << sub_proof_indent(indent_level) << "┌ " << proof[i].expr.to_string() << " : " << rule_to_string_plain(proof[i].rule) << "\n";
            proof[i].line_number = line_num;
            to_plain_subproof(file, proof[i].subproof.value(), line_num, ++indent_level);
        } else { // Regular Step
            if (indent_level > 0 && (i+1 == proof.size())) {
                subproof_end = "└ ";
                indent_level--;
            }
            file << line_num++ << ". " << sub_proof_indent(indent_level) << subproof_end << proof[i].expr.to_string() << " : " << rule_to_string_plain(proof[i].rule) << reference_print(proof[i].references.value()) << "\n";
            proof[i].line_number = line_num;
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
        file << ++line_num << "  " << indent << proof.premises[i].expr.to_string() << " : " << "Premise" << "\n";
        proof.premises[i].line_number = line_num;
    }
    to_plain_subproof(file, proof.proof, ++line_num, indent_level);

    file.close();
}