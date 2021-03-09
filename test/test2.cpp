/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#include "boolexpr.h"
#include <iostream>
#include <fmt/core.h>

int main(int argc, char *argv[]) {
    std::string input;
    nnplib::boolexpr expr;
    fmt::print("Expression: ");
    while (std::getline(std::cin, input)) {
        try {
            expr.parse(input);
            fmt::print("RPN: {}\n", expr.extract());
            auto table = expr.get_truth_table();
            fmt::print("Truth table: {{ ");
            for (auto val: table) fmt::print("{:d}, ", val);
            fmt::print("\b\b }}\n");
            fmt::print("MINTERMS m = {{ ");
            for (int i = 0; i < table.size(); ++i) {
                if (table[i]) fmt::print("{}, ", i);
            }
            fmt::print("\b\b }}\n");
            fmt::print("MAXTERMS M = {{ ");
            for (int i = 0; i < table.size(); ++i) {
                if (!table[i]) fmt::print("{}, ", i);
            }
            fmt::print("\b\b }}\n");
        } catch (std::runtime_error &e) {
            fmt::print("{}\n", e.what());
        }
        fmt::print("Expression: ");
        input.clear();
    }
    return 0;
}
