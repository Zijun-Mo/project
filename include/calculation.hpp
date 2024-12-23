#ifndef CALCULATION_HPP
#define CALCULATION_HPP
#include <bits/stdc++.h>
#include "Lexer.hpp"

// 定义计算器类
class Calculation {
public:
    // 构造函数
    Calculation(const std::vector<Token>& tokens); 
    // 执行计算
    double execute();
    //字符串转数字
    double stringToNumber(const std::string& str);
    // 中缀转后缀
    std::vector<Token> infixToPostfix(const std::vector<Token>& infix);
    // 计算后缀表达式
    double evaluatePostfix(const std::vector<Token>& postfix);

private:
    std::vector<Token> tokens; // 标记序列
};
#endif // CALCULATION_HPP