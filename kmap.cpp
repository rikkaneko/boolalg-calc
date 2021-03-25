/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#include "kmap.h"
#include <stdexcept>
#include <cmath>

namespace nnplib {
    int kmap::from_truthtable(const std::vector<bool> &table, const std::vector<char> &var_list) {
        reset();
        order_ = (uint) std::log2(table.size());
        set_var_list(var_list);
        for (uint i = 0; i < table.size(); ++i) {
            if (table[i]) minterms_.emplace_back(i);
        }
        return 0;
    }
    
    int kmap::from_truthtable(const std::string &bits, const std::vector<char> &var_list) {
        reset();
        order_ = (uint) std::log2(bits.size());
        set_var_list(var_list);
        for (uint i = 0; i < bits.size(); ++i) {
            switch (bits[i]) {
            case '0':
                break;
            case '1':
                minterms_.emplace_back(i);
                break;
            case 'x':
            case 'X':
            case '?':
                dterms_.emplace_back(i);
                break;
            default:
                reset();
                throw std::runtime_error(std::string() + "Illegal character `" + bits[i] + "`.");
            }
        }
        return 0;
    }
    
    int kmap::from_minterm(const std::vector<uint> &minterms, const std::vector<uint> &dterms, uint var_n,
                           const std::vector<char> &var_list) {
        //reset();
        order_ = var_n;
        set_var_list(var_list);
        minterms_ = minterms;
        dterms_ = dterms;
        return 0;
    }
    
    int kmap::set_var_list(const std::vector<char> &var_list) {
        if (var_list.empty()) {
            var_list_.reserve(order_);
            for (int i = 0; i < order_; ++i) var_list_.push_back('A' + (char) i);
            return order_;
        } else {
            if (order_ != var_list.size()) throw std::runtime_error("Mismatch number of variables.");
            var_list_ = var_list;
        }
        return 0;
    }
    
    std::vector<std::string> kmap::infuse_terms() {
        std::vector<std::string> expr;
        for (auto &t: prime_term_) {
            std::string term;
            for (int i = 0; i < order_; ++i) {
                switch (t.content[i]) {
                case '1':
                    term.push_back(var_list_[i]);
                    break;
                case '0':
                    term.push_back('~');
                    term.push_back(var_list_[i]);
                case '-':
                    break;
                default:
                    throw std::runtime_error(
                            std::string() + "Illegal character `" + t.content[i] + "` in the term.");
                }
            }
            expr.emplace_back(std::move(term));
        }
        return expr;
    }
    
    std::optional<kmap::term> kmap::combine(const term &t1, const term &t2) const {
        bool diff = false;
        kmap::term merged = t1;
        for (int i = 0; i < order_; ++i) {
            if (merged.content[i] != t2.content[i]) {
                if (diff) return {};
                if (merged.content[i] == '1') --merged.one_co;
                merged.content[i] = '-';
                diff = true;
            }
        }
        merged.dterm &= t2.dterm;
        merged.pair.insert(t2.pair.begin(), t2.pair.end());
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
        t.pair.emplace(val);
        return t;
    }
    
    void kmap::reset() {
        minterms_.clear();
        dterms_.clear();
        var_list_.clear();
        order_ = 0;
    }
    
    std::vector<std::string> kmap::optimize() {
        if (!order_) return {};
        const int times = 1 << order_;
        std::vector<std::vector<term>> terms(order_ + 1);
        std::vector<std::vector<term>> terms_nextorder(order_ + 1);
        std::vector<term> prime_term_candidate;
        std::vector<int> appeared(times);
        auto is_no_term_left = [&terms]() -> bool {
            for (auto &v: terms) {
                if (!v.empty()) return false;
            }
            return true;
        };
        
        auto is_term_exist = [&terms_nextorder](const term &t2) -> bool {
            for (auto &v: terms_nextorder) {
                for (auto &t1: v) {
                    if (t1.pair == t2.pair) return true;
                }
            }
            return false;
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
        for (int i = 2; i <= times && !is_no_term_left(); i *= 2) {
            for (int j = 0; j < order_; ++j) {
                for (auto &t1: terms[j]) {
                    for (auto &t2: terms[j + 1]) {
                        if (auto merged = combine(t1, t2)) {
                            if (is_term_exist(*merged)) continue;
                            for (auto &v: merged->pair) ++appeared[v];
                            terms_nextorder[merged->one_co].emplace_back(std::move(*merged));
                        }
                    }
                }
            }
            
            for (auto &v: terms) {
                for (auto &t: v) {
                    bool prime = false;
                    for (auto &m: t.pair) {
                        if (appeared[m] == 0) {
                            prime = true;
                            break;
                        }
                    }
                    if (prime && !t.dterm) prime_term_candidate.emplace_back(std::move(t));
                }
            }
            
            terms = std::move(terms_nextorder);
            terms_nextorder.clear();
            terms_nextorder.resize(order_ + 1);
            for (auto &v: appeared) v = 0;
        }
        
        for (auto &v: terms) {
            for (auto &t: v) prime_term_candidate.emplace_back(std::move(t));
        }
        
        for (auto &t: prime_term_candidate) {
            for (auto &min: minterms_) {
                if (t.pair.contains(min)) ++appeared[min];
            }
        }
        
        for (auto &t: prime_term_candidate) {
            for (auto &m: t.pair) {
                if (appeared[m] == 1) {
                    prime_term_.emplace_back(std::move(t));
                    break;
                }
            }
        }
        
        // Petrick's method
        
        
        
        return infuse_terms();
    }
    
    std::vector<std::string> kmap::optimize(const std::string &expression) {
        boolexpr expr;
        expr.parse(expression);
        from_truthtable(expr.get_truth_table(), expr.get_var_list());
        return optimize();
    }
    
    int kmap::count_one(const kmap::term &t) {
        int one_co = 0;
        for (auto &c: t.content) {
            if (c == '1') ++one_co;
        }
        return one_co;
    }
    
    std::vector<kmap::term> kmap::extract() const {
        return prime_term_;
    }
}
