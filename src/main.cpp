#include "Expr.hpp"
#include "TruthTree.hpp"
#include "parse.hpp"
#include "errors.hpp"
#include "aris.hpp"

#include <fstream>
#include <iostream>
#include <string>

enum EXIT_STATUS {
    VALID_PROOF,
    INVALID_PROOF,
    PARSING_ERROR
};

int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Incorrect Usage\nUSAGE: " << argv[0] << " <argument file>\n";
        return PARSING_ERROR;
    }

    std::ifstream in_file(argv[1]);
    ASSERT(in_file.is_open());

    int exit_status;

    std::vector<Expr *> expressions;
    std::string line;
    while (std::getline(in_file, line)) {
        Expr *expr = parse(line);
        if (!expr) {
            std::cout << "Error Parsing File\n";
            return PARSING_ERROR;
        }
        expressions.push_back(expr);
    }

    expressions[expressions.size()-1]->negate();

    std::pair<TruthNode *, int> truth_tree = compute_truth_tree(expressions);

    if (truth_tree.first) {
        std::cout << "Argument is valid\n";
        Proof proof = to_proof(truth_tree);
        to_plain(proof);
        to_aris(proof);
        exit_status = VALID_PROOF;
    } else {
        std::cout << "Argument is invalid\n";
        exit_status = INVALID_PROOF;
    }

    for (Expr *e: expressions) {
        std::cout << *e << "\n";
        delete e;
    }

    delete_truth_tree(truth_tree.first);

    return exit_status;
}
