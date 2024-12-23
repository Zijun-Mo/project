#ifndef PARSER_HPP
#define PARSER_HPP
#include "Lexer.hpp"
#include <bits/stdc++.h>
#include <cstdlib>
#include <unistd.h>

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), position(0) {}
    void setOutputFile(const std::string& filename) {
        outputfile = filename;
    }

    void parse(); // 解析入口

private:
    std::vector<Token> tokens;
    size_t position;
    struct Condition {
        std::string column;
        std::string op;
        std::string value;
        std::string logicOp; // AND 或 OR
    };
    struct Join {
        std::string leftTableAlias;
        std::string leftColumn;
        std::string rightTableAlias;
        std::string rightColumn;
    };

    // 主解析方法，根据不同的命令调用相应的解析函数
    void parseStatement();

    // 各种命令的解析函数
    void parseCreateDatabase();
    void parseUseDatabase();
    void parseCreateTable();
    void parseDropTable();
    void parseInsert();
    void parseSelect();
    void parseUpdate();
    void parseDelete();
    std::vector<Condition> parseWhere(Token tableName);
    bool evaluateConditions(const std::vector<std::string>& row, const std::vector<Condition>& conditions, const std::vector<int>& whereIndices); 

    // 辅助解析函数
    void executeSelect(
    const std::string& mainTableName,
    const std::vector<Join>& joins,
    const std::vector<std::string>& selectList,
    bool selectAll,
    const std::vector<Condition>& conditions,
    const std::map<std::string, std::string>& tableAliases); 
    Token expect(TokenType type, const std::string& value = "");
    bool match(TokenType type, const std::string& value = "");
    Token consume();
    bool loadTableData(const std::string& tableName, std::vector<std::string>& headers, std::vector<std::vector<std::string>>& data); 
    int getColumnIndex(const std::vector<std::string>& headers, const std::string& columnName);

    // 错误处理
    void error(const std::string& message);
    std::string tokenTypeToString(TokenType type); // 将 TokenType 转换为字符串
    std::string outputfile; // 输出文件名
}; 
#endif