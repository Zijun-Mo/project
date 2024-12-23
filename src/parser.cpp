#include "parser.hpp"
#include "calculation.hpp"
#include "Global.hpp"
#include <bits/stdc++.h>
#include <vector>
void Parser::parse() {
    while (position < tokens.size() && tokens[position].type != END_OF_ORDER) {
        parseStatement();
    }
}
void Parser::parseStatement() {
    if (match(KEYWORD, "CREATE")) {
        consume(); // 消费 'CREATE'
        if (match(KEYWORD, "DATABASE")) {
            parseCreateDatabase();
        } else if (match(KEYWORD, "TABLE")) {
            parseCreateTable();
        } else {
            error("Expected 'DATABASE' or 'TABLE' after 'CREATE'");
        }
    } else if (match(KEYWORD, "USE")) {
        parseUseDatabase();
    } else if (match(KEYWORD, "DROP")) {
        consume(); // 消费 'DROP'
        if (match(KEYWORD, "TABLE")) {
            parseDropTable();
        } else {
            error("Expected 'TABLE' after 'DROP'");
        }
    } else if (match(KEYWORD, "INSERT")) {
        parseInsert();
    } else if (match(KEYWORD, "SELECT")) {
        parseSelect();
    } else if (match(KEYWORD, "UPDATE")) {
        parseUpdate();
    } else if (match(KEYWORD, "DELETE")) {
        parseDelete();
    } else {
        error("Unknown statement");
    }
}
void Parser::parseCreateDatabase() {
    consume(); // 消费 'DATABASE'
    Token dbName = expect(IDENTIFIER); // 获取数据库名称
    expect(END_OF_ORDER); // 消费 ';'
    std::string current_path = Global::directoryPath.string();
    // 构建命令字符串
    std::string command = "mkdir " + current_path + "/" + dbName.value;

    // 调用系统命令创建目录
    int ret = system(command.c_str());
    if (ret == -1) {
        std::cerr << "Error executing command: " << command << std::endl;
    } else {
        std::cout << "Database created: " << dbName.value << std::endl;
    }
}
void Parser::parseUseDatabase() {
    consume(); // 消费 'USE'
    consume(); // 消费 'DATABASE'
    Token dbName = expect(IDENTIFIER); // 获取数据库名称
    expect(END_OF_ORDER); // 消费 ';'
    std::filesystem::path aim_path = Global::directoryPath / dbName.value;

    // 使用 chdir 改变当前工作目录
    if (chdir(aim_path.c_str()) != 0) {
        std::cerr << "无法切换到数据库目录：" << aim_path << std::endl;
    } else {
        std::cout << "已切换到数据库：" << dbName.value << std::endl;
    }
}
void Parser::parseCreateTable() {
    consume(); // 消费 'TABLE'
    Token tableName = expect(IDENTIFIER); // 获取表名
    expect(SYMBOL, "("); // 消费 '('

    // 解析列定义
    std::vector<std::pair<std::string, std::string>> columns;
    do {
        Token columnName = expect(IDENTIFIER); // 列名
        Token dataType = expect(KEYWORD); // 数据类型 (FLOAT, TEXT, INTEGER)
        columns.push_back({columnName.value, dataType.value}); // 保存列定义

        if (match(SYMBOL, ",")) {
            consume(); // 消费 ','
        } else {
            break;
        }
    } while (true);

    expect(SYMBOL, ")"); // 消费 ')'
    expect(END_OF_ORDER); // 消费 ';'

    // 构建命令字符串   
    std::string command = "touch " + tableName.value + ".csv";

    // 调用系统命令创建 CSV 文件
    int ret = system(command.c_str());
    if (ret == -1) {
        std::cerr << "Error executing command: " << command << std::endl;
    } else {
        std::cout << "Table created: " << tableName.value << ".csv" << std::endl;

        // 打开 CSV 文件并写入列定义
        std::ofstream file(tableName.value + ".csv");
        if (file.is_open()) {
            for (const auto& column : columns) {
                file << column.second << ",";
            }
            file.seekp(-1, std::ios_base::end); // 移动到最后一个逗号的位置
            file << "\n"; // 换行
            for (const auto& column : columns) {
                file << column.first << ",";
            }
            file.seekp(-1, std::ios_base::end); // 移动到最后一个逗号的位置
            file << "\n"; // 换行
            file.close();
        } else {
            std::cerr << "Error opening file: " << tableName.value << ".csv" << std::endl;
        }
    }

}
void Parser::parseDropTable() {
    consume(); // 消费 'TABLE'
    Token tableName = expect(IDENTIFIER); // 表名
    expect(END_OF_ORDER); // 消费 ';'

    // 构建命令字符串
    std::string command = "rm " + tableName.value + ".csv";

    // 调用系统命令删除 CSV 文件
    int ret = system(command.c_str());
    if(ret == -1) {
        std::cerr << "Error executing command: " << command << std::endl;
    } else {
        std::cout << "Table dropped: " << tableName.value << ".csv" << std::endl;
    }
}
void Parser::parseInsert() {
    consume(); // 消费 'INSERT'
    expect(KEYWORD, "INTO"); // 消费 'INTO'
    Token tableName = expect(IDENTIFIER); // 表名
    expect(KEYWORD, "VALUES"); // 消费 'VALUES'
    expect(SYMBOL, "("); // 消费 '('

    // 解析值列表
    std::vector<Token> values;
    do {
        Token value;
        if (match(NUMBER)) {
            value = consume(); // 数字
        } else if (match(STRING)) {
            value = consume(); // 字符串
        } else {
            error("Expected NUMBER or STRING value");
        }
        values.push_back(value);

        if (match(SYMBOL, ",")) {
            consume(); // 消费 ','
        } else {
            break;
        }
    } while (true);

    expect(SYMBOL, ")"); // 消费 ')'
    expect(END_OF_ORDER); // 消费 ';'
    // 打开csv文件确定数据类型
    std::ifstream csvFile(tableName.value + ".csv");
    if(csvFile.is_open()){
        std::string line;
        std::vector<std::string> dataTypes;
        std::getline(csvFile, line);
        std::istringstream ss(line);
        std::string dataType;
        while (std::getline(ss, dataType, ',')) {
            dataTypes.push_back(dataType);
        }
        std::getline(csvFile, line);
        ss = std::istringstream(line);
        std::string header;
        std::vector<std::string> headers;
        while (std::getline(ss, header, ',')) {
            headers.push_back(header);
        }
        if(headers.size() != values.size()){
            std::cerr << "Values count does not match column count" << std::endl;
            return;
        }
        for(int i = 0; i < values.size(); i++){
            if(dataTypes[i] == "INTEGER"){
                if(!std::all_of(values[i].value.begin(), values[i].value.end(), ::isdigit)){
                    std::cerr << "Value " << values[i].value << " is not a valid INTEGER" << std::endl;
                    return;
                }
            }
            else if(dataTypes[i] == "FLOAT"){
                try {
                    float f = std::stof(values[i].value);
                    // 保留两位小数
                    std::ostringstream oss = std::ostringstream(); 
                    oss << std::fixed << std::setprecision(2) << f;
                    values[i].value = oss.str();
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Value " << values[i].value << " is not a valid FLOAT" << std::endl;
                    return;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Value " << values[i].value << " is out of range for FLOAT" << std::endl;
                    return;
                }
            }
            else if(dataTypes[i] == "TEXT"){
                values[i].value = "'" + values[i].value + "'";
            }
            else{
                std::cerr << "Unknown data type: " << dataTypes[i] << std::endl;
                return;
            }
        }
    }

    // 打开 CSV 文件并写入值
    std::ofstream file(tableName.value + ".csv", std::ios::app); // 以追加模式打开文件
    if (file.is_open()) {
        for (const auto& value : values) {
            file << value.value << ",";
        }
        file.seekp(-1, std::ios_base::end); // 移动到最后一个逗号的位置
        file << "\n"; // 换行
        file.close();
        std::cout << "Values inserted into table: " << tableName.value << std::endl;
    } else {
        std::cerr << "Error opening file: " << tableName.value << ".csv" << std::endl;
    }
}
void Parser::parseSelect() {
    consume(); // 消费 'SELECT'

    // 解析 select 列表
    std::vector<std::string> selectList;
    bool selectAll = false;
    if (match(SYMBOL, "*")) {
        consume(); // 消费 '*'
        selectAll = true;
    } else {
        do {
            Token column = expect(IDENTIFIER);
            std::string tableAlias;
            std::string columnName = column.value;

            if (match(SYMBOL, ".")) {
                consume(); // 消费 '.'
                Token column2 = expect(IDENTIFIER);
                tableAlias = column.value;
                columnName = column2.value;
            }

            if (!tableAlias.empty()) {
                selectList.push_back(tableAlias + "." + columnName);
            } else {
                selectList.push_back(columnName);
            }

            if (match(SYMBOL, ",")) {
                consume(); // 消费 ','
            } else {
                break;
            }
        } while (true);
    }

    expect(KEYWORD, "FROM"); // 消费 'FROM'
    Token tableName = expect(IDENTIFIER); // 主表名

    // 处理可选的表别名
    std::string mainTableAlias = tableName.value;
    if (match(IDENTIFIER)) {
        Token alias = consume();
        mainTableAlias = alias.value;
    }
    // 存储表别名和表名的映射
    std::map<std::string, std::string> tableAliases;
    tableAliases[mainTableAlias] = tableName.value;

    // 检查是否存在 INNER JOIN
    std::vector<Join> joins;
    while (match(KEYWORD, "INNER")) {
        consume(); // 消费 'INNER'
        expect(KEYWORD, "JOIN"); // 消费 'JOIN'
        Token joinTable = expect(IDENTIFIER); // 要连接的表名

        // 可选的表别名
        std::string joinTableAlias = joinTable.value;
        if (match(IDENTIFIER)) {
            Token alias = consume();
            joinTableAlias = alias.value;
        }
        tableAliases[joinTableAlias] = joinTable.value;

        expect(KEYWORD, "ON"); // 消费 'ON'

        // 解析连接条件，例如 table1.column1 = table2.column2
        Token leftOperandTable = expect(IDENTIFIER);
        expect(SYMBOL, "."); // 消费 '.'
        Token leftOperandColumn = expect(IDENTIFIER);
        expect(SYMBOL, "="); // 消费 '='
        Token rightOperandTable = expect(IDENTIFIER);
        expect(SYMBOL, "."); // 消费 '.'
        Token rightOperandColumn = expect(IDENTIFIER);

        // 保存连接信息
        joins.push_back({
            leftOperandTable.value, leftOperandColumn.value,
            rightOperandTable.value, rightOperandColumn.value
        });
    }
    // 解析可选的 WHERE 子句
    std::vector<Condition> conditions;
    if (match(KEYWORD, "WHERE")) {
        conditions = parseWhere(tableName);
    }

    expect(END_OF_ORDER); // 消费 ';'

    // 执行 SELECT 查询

    executeSelect(mainTableAlias, joins, selectList, selectAll, conditions, tableAliases);
}
void Parser::executeSelect(
    const std::string& mainTableAlias,
    const std::vector<Join>& joins,
    const std::vector<std::string>& selectList,
    bool selectAll,
    const std::vector<Condition>& conditions,
    const std::map<std::string, std::string>& tableAliases) {

    // 加载所有涉及的表数据
    std::map<std::string, std::vector<std::string>> tableHeaders;
    std::map<std::string, std::vector<std::vector<std::string>>> tableData;
    bool is_inner_join = false; 

    for (const auto& [alias, realName] : tableAliases) {
        std::vector<std::string> headers;
        std::vector<std::vector<std::string>> data;
        if (!loadTableData(realName, headers, data)) {
            return;
        }
        // 为表头添加表别名作为前缀
        for (auto& header : headers) {
            header = alias + "." + header;
        }
        tableHeaders[alias] = headers;
        tableData[alias] = data;
    }

    // 将主表的数据和表头初始化
    std::vector<std::vector<std::string>> resultData = tableData[mainTableAlias];
    std::vector<std::string> resultHeaders = tableHeaders[mainTableAlias];

    // 执行 INNER JOIN 操作
    for (const auto& join : joins) {
        is_inner_join = true;
        std::vector<std::vector<std::string>> newResult;

        int leftIndex = getColumnIndex(resultHeaders, join.leftTableAlias + "." + join.leftColumn);
        int rightIndex = getColumnIndex(tableHeaders[join.rightTableAlias], join.rightTableAlias + "." + join.rightColumn);

        if (leftIndex == -1 || rightIndex == -1) {
            std::cerr << "JOIN 条件中的列未找到。" << std::endl;
            return;
        }

        for (const auto& row1 : resultData) {
            for (const auto& row2 : tableData[join.rightTableAlias]) {
                if (row1[leftIndex] == row2[rightIndex]) {
                    // 合并两行数据
                    std::vector<std::string> combinedRow = row1;
                    combinedRow.insert(combinedRow.end(), row2.begin(), row2.end());
                    newResult.push_back(combinedRow);
                }
            }
        }

        // 更新结果数据和表头
        resultData = newResult;
        resultHeaders.insert(resultHeaders.end(),
                             tableHeaders[join.rightTableAlias].begin(),
                             tableHeaders[join.rightTableAlias].end());
    }

    // 查找 WHERE 条件中的列索引
    std::vector<int> whereIndices;
    for (const auto& condition : conditions) {
        int index = getColumnIndex(resultHeaders, condition.column);
        if (index != -1) {
            whereIndices.push_back(index);
        } else {
            std::cerr << "未找到列：" << condition.column << std::endl;
            return;
        }
    }

    // 过滤数据
    std::vector<std::vector<std::string>> filteredData;
    for (const auto& row : resultData) {
        bool match = evaluateConditions(row, conditions, whereIndices);
        if (match) {
            filteredData.push_back(row);
        }
    }

    // 查找 SELECT 列的索引
    std::vector<int> selectIndices;
    if (selectAll) {
        for (size_t i = 0; i < resultHeaders.size(); ++i) {
            selectIndices.push_back(i);
        }
    } else {
        for (const auto& col : selectList) {
            int index; 
            if(!is_inner_join) index = getColumnIndex(resultHeaders, mainTableAlias + "." + col);
            else index = getColumnIndex(resultHeaders, col);
            if (index != -1) {
                selectIndices.push_back(index);
            } else {
                std::cerr << "未找到列：" << col << std::endl;
                return;
            }
        }
    }

    // 输出结果到文件
    std::ofstream outFile(outputfile, std::ios::app); // 以追加模式打开文件
    if (!outFile.is_open()) {
        std::cerr << "无法打开输出文件：" << outputfile << std::endl;
        return;
    }

    // 输出表头
    for (size_t i = 0; i < selectIndices.size(); ++i) {
        std::string header = resultHeaders[selectIndices[i]];
        if(!is_inner_join){
            size_t dotPos = header.find('.');
            if (dotPos != std::string::npos) {
                header = header.substr(dotPos + 1);
            }
        }
        outFile << header;
        if (i < selectIndices.size() - 1) {
            outFile << ",";
        }
    }
    outFile << "\n";

    // 输出数据行
    for (const auto& row : filteredData) {
        for (size_t i = 0; i < selectIndices.size(); ++i) {
            outFile << row[selectIndices[i]];
            if (i < selectIndices.size() - 1) {
                outFile << ",";
            }
        }
        outFile << "\n";
    }
    outFile << "---\n"; // 用于分隔不同的查询结果
    outFile.close();
}
void Parser::parseUpdate() {
    consume(); // 消费 'UPDATE'
    Token tableName = expect(IDENTIFIER); // 表名
    expect(KEYWORD, "SET"); // 消费 'SET'

    // 解析赋值列表
    std::vector<std::pair<std::string, std::vector<Token>>> assignments;
    do {
        Token column = expect(IDENTIFIER);
        expect(SYMBOL, "="); // 消费 '='
        std::vector<Token> expression = {};
        Token value;
        if (match(STRING)) {
            value = consume(); // 消费
            expression.push_back(value);
        }
        while ((match(SYMBOL) || match(NUMBER) || match(IDENTIFIER)) && !match(SYMBOL, ",") && !match(KEYWORD, "WHERE") && !match(END_OF_ORDER) && !match(SYMBOL, ";")) {
            value = consume(); // 消费
            expression.push_back(value);
        }
        assignments.push_back({column.value, expression}); // 保存赋值

        if (match(SYMBOL, ",")) {
            consume(); // 消费 ','
        } else {
            if(match(SYMBOL, ";") || match(END_OF_ORDER) || match(KEYWORD, "WHERE")){
                break;
            }
            else{
                error("Expected ',' but got '" + value.value + "'");
            }
        }
    } while (true);

    // 解析可选的 WHERE 子句
    std::vector<Condition> conditions;
    if (match(KEYWORD, "WHERE")) {
        conditions = parseWhere(tableName);
    }

    expect(END_OF_ORDER); // 消费 ';'

    // 打开 CSV 文件并读取数据
    std::ifstream file(tableName.value + ".csv");
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << tableName.value << ".csv" << std::endl;
        return;
    }
    std::string dataTypeLine;
    if (!std::getline(file, dataTypeLine)) {
        std::cerr << "读取数据类型行失败：" << tableName.value << ".csv" << std::endl;
        return;
    }
    std::string line;
    std::vector<std::string> headers;
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string header;
        while (std::getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }

    // 查找赋值列的索引
    std::vector<int> assignmentIndices;
    for (const auto& assignment : assignments) {
        auto it = std::find(headers.begin(), headers.end(), assignment.first);
        if (it != headers.end()) {
            assignmentIndices.push_back(std::distance(headers.begin(), it));
        } else {
            std::cerr << "Column not found: " << assignment.first << std::endl;
            return;
        }
    }

    // 查找 where 列的索引
    std::vector<int> whereIndices;
    for (const auto& condition : conditions) {
        std::string column = condition.column.substr(condition.column.find('.') + 1); 
        auto it = std::find(headers.begin(), headers.end(), column);
        if (it != headers.end()) {
            whereIndices.push_back(std::distance(headers.begin(), it));
        } else {
            std::cerr << "Column not found: " << condition.column << std::endl;
            return;
        }
    }

    // 读取数据并更新
    std::vector<std::vector<std::string>> rows;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::vector<std::string> row;
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        // 筛选条件
        bool match = evaluateConditions(row, conditions, whereIndices);

        if (match) {
            // 更新赋值
            for (size_t i = 0; i < assignments.size(); ++i) {
                if(assignments[i].second.size() == 1 && assignments[i].second[0].type == STRING){
                    row[assignmentIndices[i]] = "'" + assignments[i].second[0].value + "'"; 
                    continue;
                }
                std::vector<Token> tokens = assignments[i].second;
                for(auto &token: tokens){
                    if(token.type == IDENTIFIER){
                        token.type = NUMBER; 
                        token.value = row[assignmentIndices[i]];
                    }
                }
                Calculation calc(tokens); 
                if(row[assignmentIndices[i]].find('.') == std::string::npos){
                    row[assignmentIndices[i]] = std::to_string(int(calc.execute())); 
                }
                else{
                    std::stringstream ss = std::stringstream();
                    ss << std::fixed << std::setprecision(2) << calc.execute();
                    row[assignmentIndices[i]] = ss.str();
                }
            }
        }

        rows.push_back(row);
    }

    file.close();

    // 写回更新后的数据
    std::ofstream outFile(tableName.value + ".csv");
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << tableName.value << ".csv" << std::endl;
        return;
    }
    // 写入数据类型行
    outFile << dataTypeLine << "\n";
    // 写入表头
    for (size_t i = 0; i < headers.size(); ++i) {
        outFile << headers[i] << ",";
    }
    outFile.seekp(-1, std::ios_base::end); // 移动到最后一个逗号的位置
    outFile << "\n";

    // 写入数据行
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            outFile << row[i];
            if (i < row.size() - 1) {
                outFile << ",";
            }
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Table updated: " << tableName.value << ".csv" << std::endl;
}
void Parser::parseDelete() {
    consume(); // 消费 'DELETE'
    expect(KEYWORD, "FROM"); // 消费 'FROM'
    Token tableName = expect(IDENTIFIER); // 表名

    // 解析可选的 WHERE 子句
    std::vector<Condition> conditions;
    if (match(KEYWORD, "WHERE")) {
        conditions = parseWhere(tableName);
    }

    expect(END_OF_ORDER); // 消费 ';'

    // 打开 CSV 文件并读取数据
    std::ifstream file(tableName.value + ".csv");
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << tableName.value << ".csv" << std::endl;
        return;
    }

    std::string line, dataTypeLine;
    std::getline(file, dataTypeLine); // 跳过数据类型行
    std::vector<std::string> headers;
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string header;
        while (std::getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }

    // 查找 where 列的索引
    std::vector<int> whereIndices;
    for (const auto& condition : conditions) {
        std::string column = condition.column.substr(condition.column.find('.') + 1);
        auto it = std::find(headers.begin(), headers.end(), column);
        if (it != headers.end()) {
            whereIndices.push_back(std::distance(headers.begin(), it));
        } else {
            std::cerr << "Column not found: " << condition.column << std::endl;
            return;
        }
    }

    // 读取数据并删除
    std::vector<std::vector<std::string>> rows;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::vector<std::string> row;
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        // 筛选条件
        bool match = evaluateConditions(row, conditions, whereIndices);

        if (!match) {
            rows.push_back(row); // 仅保留不匹配的行
        }
    }

    file.close();

    // 写回删除后的数据
    std::ofstream outFile(tableName.value + ".csv");
    outFile << dataTypeLine << "\n";
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << tableName.value << ".csv" << std::endl;
        return;
    }

    // 写入表头
    for (const auto& header : headers) {
        outFile << header << ",";
    }
    outFile.seekp(-1, std::ios_base::end); // 移动到最后一个逗号的位置
    outFile << "\n"; // 换行

    // 写入数据行
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            outFile << row[i] << ",";
        }
        outFile.seekp(-1, std::ios_base::end); // 移动到最后一个逗号的位置
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Rows deleted from table: " << tableName.value << std::endl;
}
// 解析 WHERE 子句
std::vector<Parser::Condition> Parser::parseWhere(Token tableName) {
    std::vector<Condition> conditions;
    consume(); // 消费 'WHERE'
    do {
        // 解析条件
        Condition condition;
        condition.column = expect(IDENTIFIER).value;
        Token op = consume(); 
        if(op.value == "."){
            condition.column = condition.column + "." + expect(IDENTIFIER).value;
            condition.op = consume().value;
        }
        else{
            condition.column = tableName.value + "." + condition.column;
            condition.op = op.value; // 比较操作符，例如 '=', '<', '>'
        }
        Token value = consume();
        condition.value = value.value;
        condition.logicOp = ""; // 默认没有逻辑操作符

        // 检查是否有 AND 或 OR
        if (match(KEYWORD, "AND")) {
            consume(); // 消费 'AND'
            condition.logicOp = "AND";
        } else if (match(KEYWORD, "OR")) {
            consume(); // 消费 'OR'
            condition.logicOp = "OR";
        }

        conditions.push_back(condition);

        // 如果没有 AND 或 OR，则结束条件解析
        if (condition.logicOp.empty()) {
            break;
        }
    } while (true);

    return conditions;
}

// 评估条件
bool Parser::evaluateConditions(const std::vector<std::string>& row, const std::vector<Condition>& conditions, const std::vector<int>& whereIndices) {
    bool result = true;
    for (size_t i = 0; i < conditions.size(); ++i) {
        const auto& condition = conditions[i];
        int whereIndex = whereIndices[i];
        bool match = false;

        if (condition.op == "=") {
            if(row[whereIndex][0] == '\''){
                match = (row[whereIndex].substr(1, row[whereIndex].size() - 2) == condition.value);
            }
            else match = (row[whereIndex] == condition.value);
        } else if (condition.op == "<") {
            match = (std::stod(row[whereIndex]) < std::stod(condition.value));
        } else if (condition.op == ">") {
            match = (std::stod(row[whereIndex]) > std::stod(condition.value));
        } else if (condition.op == "<=") {
            match = (std::stod(row[whereIndex]) <= std::stod(condition.value));
        } else if (condition.op == ">=") {
            match = (std::stod(row[whereIndex]) >= std::stod(condition.value));
        } else if (condition.op == "!=") {
            if(row[whereIndex][0] == '\''){
                match = (row[whereIndex].substr(1, row[whereIndex].size() - 2) != condition.value);
            }
            else match = (std::stod(row[whereIndex]) != std::stod(condition.value));
        } else {
            std::cerr << "Unsupported operator: " << condition.op << std::endl;
            return false;
        }

        if (i == 0) {
            result = match;
        } else {
            if (conditions[i - 1].logicOp == "AND") {
                result = result && match;
            } else if (conditions[i - 1].logicOp == "OR") {
                result = result || match;
            }
        }
    }

    return result;
}
Token Parser::expect(TokenType type, const std::string& value) {
    if (position < tokens.size()) {
        Token token = tokens[position];
        if (token.type == type && (value.empty() || token.value == value)) {
            position++;
            return token;
        } else {
            error("Expected " + tokenTypeToString(type) + " '" + value + "', but got '" + token.value + "'");
        }
    } else {
        error("Unexpected end of input");
    }
    return Token{UNKNOWN, ""}; // 为了避免编译错误
}
bool Parser::match(TokenType type, const std::string& value) {
    if (position < tokens.size()) {
        Token token = tokens[position];
        return token.type == type && (value.empty() || token.value == value);
    }
    return false;
}
Token Parser::consume() {
    if (position < tokens.size()) {
        return tokens[position++];
    }
    error("Unexpected end of input");
    return Token{UNKNOWN, ""}; // 为了避免编译错误
}
void Parser::error(const std::string& message) {
    throw std::runtime_error("Parse error at position " + std::to_string(position) + ": " + message);
}

std::string Parser::tokenTypeToString(TokenType type) {
    switch (type) {
        case KEYWORD: return "KEYWORD";
        case IDENTIFIER: return "IDENTIFIER";
        case SYMBOL: return "SYMBOL";
        case NUMBER: return "NUMBER";
        case STRING: return "STRING";
        default: return "UNKNOWN";
    }
}
bool Parser::loadTableData(const std::string& tableName, std::vector<std::string>& headers, std::vector<std::vector<std::string>>& data) {
    std::ifstream file(tableName + ".csv");
    if (!file.is_open()) {
        std::cerr << "无法打开文件：" << tableName << ".csv" << std::endl;
        return false;
    }

    std::string line, dataTypeLine;
    std::getline(file, dataTypeLine); // 跳过数据类型行
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string header;
        while (std::getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string cell;
        std::vector<std::string> row;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }

    file.close();
    return true;
}

int Parser::getColumnIndex(const std::vector<std::string>& headers, const std::string& columnName) {
    auto it = std::find(headers.begin(), headers.end(), columnName);
    if (it != headers.end()) {
        return std::distance(headers.begin(), it);
    }
    return -1;
}