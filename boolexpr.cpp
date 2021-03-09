/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#include "boolexpr.h"
#include <cctype>
#include <stack>
#include <stdexcept>
#include <set>

namespace nnplib {
    int boolexpr::parse(const std::string &expression) {
        expr_.clear();
        var_list_.clear();
        bool need_and_op = false;
        std::stack<char> op_stack;
        auto push_to_output = [&]() {
            expr_.push_back(op_stack.top());
            op_stack.pop();
        };
        
        auto add_op = [&](char c) {
            while (!op_stack.empty() && op_stack.top() != '(' && preced_op(c, op_stack.top())) push_to_output();
            op_stack.push(c);
        };
        
        auto raise_error = [&](const std::string &text) {
            expr_.clear();
            throw std::runtime_error(text);
        };
        
        for (auto c: expression) {
            if (std::isalpha(c)) {
                if (need_and_op) add_op('*');
                expr_.push_back(c);
                need_and_op = true;
            } else if (std::isspace(c)) {
                continue;
            } else if (c == '(') {
                if (need_and_op) add_op('*');
                op_stack.push('(');
                need_and_op = false;
            } else if (c == ')') {
                while (!op_stack.empty() && op_stack.top() != '(') push_to_output();
                if (!op_stack.empty() && op_stack.top() == '(') op_stack.pop();
                else raise_error("Illegal expression: unmatched left brackets.");
                need_and_op = true;
            } else if (is_valid_op(c)) {
                if (is_unary(c) && need_and_op) add_op('*');
                add_op(c);
                need_and_op = false;
            } else raise_error(std::string() + "Ilegal expression: invalid operator `" + c + "`.");
        }
        while (!op_stack.empty() && op_stack.top() != '(') push_to_output();
        if (!op_stack.empty()) raise_error("Illegal expression: unmatched right brackets.");
        return 0;
    }
    
    bool boolexpr::eval(const bitmap &bits) {
        std::stack<bool> val_stack;
        auto pop_stack = [&]() -> bool {
            bool val = val_stack.top();
            val_stack.pop();
            return val;
        };
        
        for (auto c: expr_) {
            if (std::isalpha(c)) {
                auto iter = bits.find(c);
                if (iter == bits.end())
                    throw std::runtime_error(
                            std::string() + "Illegal bitmap: value for variable `" + c + "` not found.");
                val_stack.push(iter->second);
            } else if (is_unary(c)) {
                if (val_stack.empty())
                    throw std::runtime_error(
                            std::string() + "Illegal expression: missing one operand for operator `" + c + "`.");
                bool val = pop_stack();
                val_stack.push(!val);
            } else {
                if (val_stack.size() < 2)
                    throw std::runtime_error(
                            std::string() + "Illegal expression: missing operands for operator `" + c + "`.");
                bool right_val = pop_stack();
                bool left_val = pop_stack();
                if (c == '+') val_stack.push(left_val || right_val);
                else if (c == '*') val_stack.push(left_val && right_val);
                else throw std::runtime_error(std::string() + "Ilegal expression: invalid operator `" + c + "`.");
            }
        }
        if (val_stack.size() != 1)
            throw std::runtime_error("Ilegal expression: missing operators.");
        return val_stack.top();
    }
    
    bool boolexpr::eval(bitmap &aux, uint64_t val) {
        update_var_list();
        if (var_list_.size() > 64) throw std::runtime_error("Eval: too many variables (> 64)");
        for (int i = 0; i < var_list_.size(); ++i) {
            uint k = var_list_.size() - i - 1;
            aux[var_list_[i]] = (val & (1 << k)) >> k;
        }
        return eval(aux);
    }
    
    bool boolexpr::eval(uint64_t val) {
        bitmap bits;
        return eval(bits, val);
    }
    
    std::string boolexpr::extract() const {
        return expr_;
    }
    
    bool boolexpr::preced_op(char op1, char op2) {
        switch (op1) {
        case '+':
            return true;
        case '*':
            return op2 != '+';
        case '~':
        case '!':
        default:
            return false;
        }
    }
    
    int boolexpr::is_unary(char op) {
        return op == '~' || op == '!';
    }
    
    bool boolexpr::is_valid_op(char op) {
        switch (op) {
        case '+':
        case '*':
        case '~':
        case '!':
            return true;
        default:
            return false;
        }
    }
    
    void boolexpr::update_var_list() {
        if (expr_.empty()) throw std::runtime_error("Invalid boolean expression.");
        if (var_list_.empty()) {
            std::set<char> vars;
            for (auto c: expr_) {
                if (std::isalpha(c)) vars.insert(c);
            }
            var_list_.reserve(64);
            for (auto c: vars) {
                var_list_.push_back(c);
            }
        }
    }
    
    std::vector<char> boolexpr::get_var_list() {
        update_var_list();
        return var_list_;
    }
    
    std::vector<bool> boolexpr::get_truth_table() {
        std::vector<bool> table;
        bitmap bits;
        update_var_list();
        uint64_t val = 0, max = 1 << var_list_.size();
        table.reserve(max - 1);
        while (val < max) table.push_back(eval(bits, val++));
        return table;
    }
}
