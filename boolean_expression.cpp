/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#include "boolean_expression.h"
#include <cctype>
#include <stack>
#include <stdexcept>
#include <set>

namespace nnplib {
    int boolean_expression::parse(const std::string &expression) {
        expr_.clear();
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
    
    bool boolean_expression::eval(const std::map<char, bool> &bits) {
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
    
    bool boolean_expression::eval(const std::vector<char> &order, int64_t val) {
        bitmap bits;
        if (order.size() > 64) throw std::runtime_error("Eval: too many variables (> 64)");
        for (int i = 0; i < order.size(); ++i) {
            bits.emplace(order[i], val & (1 << (order.size() - i - 1)));
        }
        return eval(bits);
    }
    
    std::string boolean_expression::extract() const {
        return expr_;
    }
    
    bool boolean_expression::preced_op(char op1, char op2) {
        switch (op1) {
        case '+':
            return true;
        case '*':
            return op2 != '+';
        case '~':
            return false;
        default:
            return false;
        }
    }
    
    int boolean_expression::is_unary(char op) {
        return op == '~' || op == '!';
    }
    
    bool boolean_expression::is_valid_op(char op) {
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
    
    std::vector<char> boolean_expression::get_var_list() const {
        std::set<char> vars;
        for (auto c: expr_) {
            if (std::isalpha(c)) vars.insert(c);
        }
        return std::vector<char>(vars.begin(), vars.end());
    }
}
