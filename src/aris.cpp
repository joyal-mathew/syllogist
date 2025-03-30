#include <iostream>
#include <fstream>
#include <string>
#include "aris.hpp"
#include "Expr.hpp"

void subproof(std::vector<Step>& proof, int& line_num, int& indent_level, int& proof_num) {
    --indent_level;
}

void to_aris(Proof& proof) {
    int indent_level = 1;
    int proof_num = 0;
    int line_num = 0;
    std::string indent(indent_level * 2, ' ');
    std::ofstream file;
    file.open("build/aris.txt");
    
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
    // subproof(proof->proof, line_num, ++indent_level, ++proof_num);

    // Ender
    file << indent << "</proof>\n";
    file << "</bram>";
    

    file.close();
}