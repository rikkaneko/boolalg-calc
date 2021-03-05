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
        bool eval(bitmap &aux, uint64_t val);
        bool eval(uint64_t val);
        void update_var_list();
        std::vector<bool> get_truth_table();
        std::vector<char> get_var_list();
        std::string extract() const;
        static bool preced_op(char op1, char op2);
        static int is_unary(char op);
        static bool is_valid_op(char op);

    private:
        std::string expr_;
        std::vector<char> var_list_;
    };
}

