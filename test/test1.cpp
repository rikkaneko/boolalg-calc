/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#include "boolean_expression.h"
#include <iostream>
#include <string>
#include <fmt/core.h>

int main(int argc, char *argv[]) {
    std::string input;
    nnplib::boolean_expression expr;
    fmt::print("Expression: ");
    while (std::getline(std::cin, input)) {
        fmt::print("Inputted: {}\n", input);
        try {
            expr.parse(input);
            fmt::print("RPN: {}\n", expr.extract());
            fmt::print("Input value by\n[1] Each\n[2] Bitmap in decimal\nchoice: ");
            std::getline(std::cin, input);
            if (input[0] == '1') {
                nnplib::boolean_expression::bitmap bits;
                for (auto c: expr.get_var_list()) {
                    fmt::print("{}: ", c);
                    std::getline(std::cin, input);
                    bits.emplace(c, input[0] != '0');
                }
                fmt::print("Result: {}\n", expr.eval(bits));
            } else if (input[0] == '2') {
                uint64_t val = 0;
                auto order = expr.get_var_list();
                fmt::print("Bitmap {{");
                for (char c: order) fmt::print("{}", c);
                fmt::print("}} in decimal: ");
                std::cin >> val;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                fmt::print("Result: {}\n", expr.eval(val));
            }
            else fmt::print("Unrecognized choice `{}`.\n", input[0]);
        } catch (std::exception &e) {
            fmt::print("{}\n", e.what());
        }
        fmt::print("Expression: ");
    }
    return 0;
}
