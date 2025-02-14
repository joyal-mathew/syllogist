#include "Expr.hpp"
#include "parse.hpp"
#include "errors.hpp"

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    ASSERT(argc == 2);
    std::ifstream in_file(argv[1]);
    ASSERT(in_file.is_open());

    std::string line;
    while (std::getline(in_file, line)) {
        Expr *expr = parse(line);
        ASSERT(expr);

        Expr *nexpr = expr->get_negation();
        Expr *expr2 = nexpr->get_unnegation();

        std::cout << *expr << std::endl;
        std::cout << *nexpr << std::endl;
        std::cout << *expr2 << std::endl;

        if (expr2->type != ExprType::Negation) {
            auto sides = expr2->decompose();
            std::cout << *sides.first << std::endl;
            std::cout << *sides.second << std::endl;

            delete sides.first;
            delete sides.second;
        }

        delete expr;
        delete nexpr;
        delete expr2;
    }

    return 0;
}
