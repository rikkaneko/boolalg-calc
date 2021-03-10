/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#pragma once
#include "boolexpr.h"
#include <vector>
#include <tuple>
#include <set>

namespace nnplib {
    class kmap {
    public:
        struct term {
            std::string content;
            std::set<uint> pair;
            bool dterm = false;
            int one_co = 0;
        };
        
        int from_truthtable(const std::vector<bool> &table, const std::vector<char> &var_list = {});
        int from_truthtable(const std::string &bits, const std::vector<char> &var_list = {});
        int from_minterm(const std::vector<uint> &minterms, const std::vector<uint> &dterms, uint var_n, const std::vector<char> &var_list = {});
        std::vector<std::string> optimize();
        std::vector<std::string> optimize(const std::string &expression);
        std::vector<term> extract() const;
        
    private:
        std::optional<term> combine(const term &t1, const term &t2) const;
        int set_var_list(const std::vector<char> &var_list);
        std::vector<std::string> infuse_terms();
        kmap::term make_term(uint val) const;
        static int count_one(const term &t);
        void reset();
        std::vector<uint> minterms_;
        std::set<uint> dterms_;
        std::vector<char> var_list_;
        std::vector<term> prime_term_;
        uint order_ = 0;
    };
}
