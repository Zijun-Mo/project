#include "calculation.hpp"
#include "Lexer.hpp"
#include <bits/stdc++.h>

// 构造函数
Calculation::Calculation(const std::vector<Token>& tokens) : tokens(tokens) {}

// 执行计算
double Calculation::execute() {
    // 将中缀表达式转换为后缀表达式
    std::vector<Token> postfix = infixToPostfix(tokens);
    // 计算后缀表达式
    return evaluatePostfix(postfix);
}

// 字符串转数字
double Calculation::stringToNumber(const std::string& str) {
    double num = 0.0;
    std::stringstream ss(str);
    ss >> num;
    return num;
}

// 中缀转后缀
std::vector<Token> Calculation::infixToPostfix(const std::vector<Token>& infix) {
    std::vector<Token> postfix;
    std::stack<Token> opStack;

    // 定义操作符优先级
    std::unordered_map<std::string, int> precedence = {
        {"(", 1},
        {"+", 2},
        {"-", 2},
        {"*", 3},
        {"/", 3}
    };

    for (const auto& token : infix) {
        if (token.type == NUMBER) {
            // 数字直接加入后缀表达式
            postfix.push_back(token);
        } else if (token.type == SYMBOL) {
            if (token.value == "(") {
                // 左括号入栈
                opStack.push(token);
            } else if (token.value == ")") {
                // 处理右括号，弹出栈中的运算符直到遇到左括号
                while (!opStack.empty() && opStack.top().value != "(") {
                    postfix.push_back(opStack.top());
                    opStack.pop();
                }
                if (opStack.empty()) {
                    throw std::runtime_error("括号不匹配");
                }
                opStack.pop(); // 弹出左括号
            } else {
                // 处理运算符
                while (!opStack.empty() && opStack.top().value != "(" &&
                       precedence[opStack.top().value] >= precedence[token.value]) {
                    postfix.push_back(opStack.top());
                    opStack.pop();
                }
                opStack.push(token);
            }
        }
    }

    // 将栈中剩余的运算符加入后缀表达式
    while (!opStack.empty()) {
        if (opStack.top().value == "(" || opStack.top().value == ")") {
            throw std::runtime_error("括号不匹配");
        }
        postfix.push_back(opStack.top());
        opStack.pop();
    }

    return postfix;
}

// 计算后缀表达式
double Calculation::evaluatePostfix(const std::vector<Token>& postfix) {
    std::stack<double> valStack;

    for (const auto& token : postfix) {
        if (token.type == NUMBER) {
            // 压入数字
            valStack.push(stringToNumber(token.value));
        } else if (token.type == SYMBOL) {
            // 弹出两个操作数
            if (valStack.size() < 2) {
                throw std::runtime_error("表达式无效");
            }
            double right = valStack.top(); valStack.pop();
            double left = valStack.top(); valStack.pop();
            double result = 0.0;

            // 计算结果
            if (token.value == "+") {
                result = left + right;
            } else if (token.value == "-") {
                result = left - right;
            } else if (token.value == "*") {
                result = left * right;
            } else if (token.value == "/") {
                if (right == 0) {
                    throw std::runtime_error("除数不能为零");
                }
                result = left / right;
            } else {
                throw std::runtime_error("未知的操作符: " + token.value);
            }
            // 压入计算结果
            valStack.push(result);
        }
    }

    // 检查最终结果
    if (valStack.size() != 1) {
        throw std::runtime_error("表达式无效");
    }

    return valStack.top();
}