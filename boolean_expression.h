/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#pragma once
#include <string>
#include <vector>
#include <map>

namespace nnplib {
    class boolean_expression {
    public:
        using bitmap = std::map<char, bool>;
        int parse(const std::string &expression);
        bool eval(const bitmap &bits);
        bool eval(const std::vector<char> &order, int64_t val);
        std::vector<char> get_var_list() const;
        std::string extract() const;
        static bool preced_op(char op1, char op2);
        static int is_unary(char op);
        static bool is_valid_op(char op);

    private:
        std::string expr_;
    };
}

