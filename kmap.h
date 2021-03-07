/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#pragma once
#include "boolean_expression.h"
#include <vector>
#include <list>
#include <tuple>
#include <set>

namespace nnplib {
    class kmap {
    public:
        struct term {
            std::string content;
            bool dterm = false;
            std::set<uint> included;
            int one_co = 0;
        };
        
        int from_truthtable(const std::vector<bool> &table);
        int from_truthtable(const std::string &bits);
        int set_variable(const std::vector<char> &var_list);
        std::list<std::string> optimize();
        std::list<std::string> optimize(const std::string &expression);
        
    private:
        std::optional<term> combine(const term &t1, const term &t2) const;
        kmap::term make_term(uint val) const;
        void reset();
        static int count_one(const term &t);
        std::vector<uint> minterms_;
        std::vector<uint> dterms_;
        std::vector<char> var_list_;
        std::list<term> prime_term_;
        int order_;
    };
}
