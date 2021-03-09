/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#include "kmap.h"
#include <stdexcept>
#include <cmath>

namespace nnplib {
    int kmap::from_truthtable(const std::vector<bool> &table) {
        reset();
        order_ = (int) std::log2(table.size()) + 1;
        for (uint i = 0; i < table.size(); ++i) {
            if (table[i]) minterms_.emplace_back(i);
        }
        return 0;
    }
    
    int kmap::from_truthtable(const std::string &bits) {
        reset();
        order_ = (int) std::log2(bits.size()) + 1;
        for (uint i = 0; i < bits.size(); ++i) {
            switch (bits[i]) {
            case '0':
                continue;
            case '1':
                minterms_.emplace_back(i);
                break;
            case 'x':
            case 'X':
                dterms_.emplace(i);
            default:
                reset();
                throw std::runtime_error(std::string() + "Illegal character `" + bits[i] + "`.");
            }
        }
        return 0;
    }
    
    int kmap::set_variable(const std::vector<char> &var_list) {
        if (order_ != var_list.size()) throw std::runtime_error("Mismatch number of variables.");
        var_list_ = var_list;
        return 0;
    }
    
    std::optional<kmap::term> kmap::combine(const term &t1, const term &t2) const {
        bool diff = false;
        kmap::term merged = t1;
        if (t1.dterm && !t2.dterm) merged.dterm = false;
        for (int i = 0; i < order_; ++i) {
            if (merged.content[i] != t2.content[i]) {
                if (diff) return {};
                if (merged.content[i] == '1') --merged.one_co;
                merged.content[i] = '-';
                diff = true;
            }
        }
        merged.included.insert(t2.included.begin(), t2.included.end());
        return merged;
    }
    
    kmap::term kmap::make_term(uint val) const {
        term t;
        for (uint i = 0; i < order_; ++i) {
            uint k = order_ - i - 1;
            bool is_one = (val & (1 << k)) >> k;
            if (is_one) ++t.one_co;
            t.content.push_back((is_one) ? '1' : '0');
        }
        t.included.emplace(val);
        return t;
    }
    
    void kmap::reset() {
        minterms_.clear();
        dterms_.clear();
        var_list_.clear();
        prime_term_.clear();
        order_ = 0;
    }
    
    std::list<std::string> kmap::optimize() {
        if (!order_) return {};
        const int times = 1 << order_;
        std::vector<std::list<term>> terms(order_ + 1);
        std::vector<int> appeared(times);
        auto empty_term = [&terms]() -> bool {
            for (auto &l: terms) {
                if (!l.empty()) return false;
            }
            return true;
        };
        
        for (auto &v: minterms_) {
            auto t = make_term(v);
            terms[t.one_co].emplace_back(t);
        }
        
        for (auto &v: dterms_) {
            auto t = make_term(v);
            t.dterm = true;
            terms[t.one_co].emplace_back(t);
        }
        
        // Quine–McCluskey algorithm
        for (int i = 2; i <= times && !empty_term(); i *= 2) {
            std::vector<std::list<term>> terms_nextorder(order_ + 1);
            for (auto &v: appeared) v = 0;
            for (int j = 0; j < order_ - 1; ++j) {
                for (auto &t1: terms[j]) {
                    for (auto &t2: terms[j + 1]) {
                        if (auto merged = combine(t1, t2)) {
                            terms_nextorder[merged->one_co].emplace_back(std::move(*merged));
                            for (auto &v: merged->included) ++appeared[v];
                        }
                    }
                }
            }
            for (auto &l: terms) {
                for (auto &t: l) {
                    bool prime = true;
                    for (auto &m: t.included) {
                        if (appeared[m] != 0) {
                            prime = false;
                            break;
                        }
                    }
                    if (prime && !t.dterm) {
                        prime_term_.emplace_back(std::move(t));
                    }
                }
            }
            terms = std::move(terms_nextorder);
        }
        
        std::list<term> candidate;
        for (auto &l: terms) {
            auto iter = l.begin();
            while (iter != l.end()) {
                bool prime = false;
                for (auto &m: iter->included) {
                    if (appeared[m] == 1 && !dterms_.contains(m)) {
                        prime_term_.emplace_back(std::move(*iter));
                        l.erase(iter++);
                        prime = true;
                        break;
                    }
                }
                if (!prime) {
                    candidate.emplace_back(std::move(*iter));
                    l.erase(iter++);
                }
            }
        }
        
        // Petrick's method
        
        
        return {};
    }
    
    std::list<std::string> kmap::optimize(const std::string &expression) {
        boolexpr expr;
        expr.parse(expression);
        from_truthtable(expr.get_truth_table());
        return optimize();
    }
    
    int kmap::count_one(const kmap::term &t) {
        int one_co = 0;
        for (auto &c: t.content) {
            if (c == '1') ++one_co;
        }
        return one_co;
    }
}
