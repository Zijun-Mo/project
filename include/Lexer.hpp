#ifndef LEXER_HPP
#define LEXER_HPP
#include <bits/stdc++.h>

// 定义标记类型
enum TokenType {
    KEYWORD,        // 关键字0
    IDENTIFIER,     // 变量1
    SYMBOL,         // 符号2
    NUMBER,         // 数字3
    STRING,         // 字符串4
    END_OF_ORDER,   // 文件结束5
    UNKNOWN         // 未知6
};

// 定义标记结构
struct Token {
    TokenType type;     // 标记类型
    std::string value;  // 标记值
};

// 关键字集合
const std::unordered_map<std::string, TokenType> keywords = {
    {"CREATE", KEYWORD}, {"DATABASE", KEYWORD}, {"USE", KEYWORD},
    {"TABLE", KEYWORD}, {"DROP", KEYWORD}, {"INSERT", KEYWORD},
    {"INTO", KEYWORD}, {"VALUES", KEYWORD}, {"SELECT", KEYWORD},
    {"FROM", KEYWORD}, {"WHERE", KEYWORD}, {"INNER", KEYWORD},
    {"JOIN", KEYWORD}, {"ON", KEYWORD}, {"UPDATE", KEYWORD},
    {"SET", KEYWORD}, {"DELETE", KEYWORD}, {"INTEGER", KEYWORD},
    {"TEXT", KEYWORD}, {"FLOAT", KEYWORD}, {"AND", KEYWORD},
    {"OR", KEYWORD}
};

// 词法分析器类
class Lexer {
public:
    // 构造函数，初始化输入字符串和当前位置
    Lexer(const std::string& input) : input(input), position(0) {}

    // 将输入字符串分解为标记
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (position < input.length()) {
            char current = input[position];
            if (isspace(current)) {
                // 跳过空白字符
                position++;
            } else if (isalpha(current)) {
                // 解析标识符或关键字
                tokens.push_back(parseIdentifierOrKeyword());
            } else if (isdigit(current)) {
                // 解析数字
                tokens.push_back(parseNumber());
            } else if (current == '\'' || current == '\"') {
                // 解析字符串
                tokens.push_back(parseString());
            } else if (isSymbol(current)) {
                // 解析符号
                tokens.push_back(parseSymbol());
            } else {
                // 解析未知字符
                tokens.push_back({UNKNOWN, std::string(1, current)});
                position++;
            }
        }
        // 添加文件结束标记
        tokens.push_back({END_OF_ORDER, ""});
        return tokens;
    }

private:
    std::string input;  // 输入字符串
    size_t position;    // 当前解析位置

    // 解析标识符或关键字
    Token parseIdentifierOrKeyword() {
        size_t start = position;
        while (position < input.length() && (isalnum(input[position]) || input[position] == '_')) {
            position++;
        }
        std::string value = input.substr(start, position - start);
        if (keywords.find(value) != keywords.end()) {
            return {keywords.at(value), value};
        }
        return {IDENTIFIER, value};
    }

    // 解析数字
    Token parseNumber() {
        size_t start = position;
        while (position < input.length() && (isdigit(input[position]) || input[position] == '.')) {
            position++;
        }
        return {NUMBER, input.substr(start, position - start)};
    }

    // 解析字符串
    Token parseString() {
        char quote = input[position];
        position++;
        size_t start = position;
        while (position < input.length() && input[position] != quote) {
            position++;
        }
        std::string value = input.substr(start, position - start);
        position++;
        return {STRING, value};
    }

    // 解析符号
    Token parseSymbol() {
        char current = input[position];
        position++;
        if(input[position] == '='){
            position++;
            return {SYMBOL, std::string(1, current) + "="};
        }
        return {SYMBOL, std::string(1, current)};
    }

    // 判断字符是否为符号
    bool isSymbol(char c) {
        return c == '(' || c == ')' || c == ',' || c == ';' || c == '=' || c == '<' || c == '>' || c == '*' || c == '.' || c == '+' || c == '-' || c == '/' || c == '!';
    }
};

#endif