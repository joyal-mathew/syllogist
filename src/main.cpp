#include "Expr.hpp"
#include "TruthTree.hpp"
#include "parse.hpp"
#include "errors.hpp"

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    ASSERT(argc == 2);
    std::ifstream in_file(argv[1]);
    ASSERT(in_file.is_open());

    std::vector<Expr *> expressions;
    std::string line;
    while (std::getline(in_file, line)) {
        Expr *expr = parse(line);
        ASSERT(expr);
        expressions.push_back(expr);
    }

    expressions[expressions.size()-1]->negate();

    std::pair<TruthNode *, int> truth_tree = compute_truth_tree(expressions);

    if (truth_tree.first) {
        std::cout << "Argument is valid\n";
        to_proof(truth_tree);
    }
    else
        std::cout << "Argument is invalid\n";

    for (Expr *e: expressions) {
        std::cout << *e << "\n";
        delete e;
    }

    delete_truth_tree(truth_tree.first);

    return 0;
}
