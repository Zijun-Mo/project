#include <bits/stdc++.h>
#include "Lexer.hpp"
#include "parser.hpp"
#include "Global.hpp"

std::string line; 
int main(int argc, char *argv[]){
    // 设置全局目录路径
    Global::directoryPath = std::filesystem::current_path(); 
    // 创建输出文件
    std::ofstream fout;
    fout.open(argv[2], std::ios::out);
    fout.close();
    // 将输入和输出文件路径转换为绝对路径
    std::filesystem::path inputPath(argv[1]);
    std::filesystem::path outputPath(argv[2]);

    inputPath = std::filesystem::absolute(inputPath);
    outputPath = std::filesystem::absolute(outputPath);

    std::ifstream fin(argv[1], std::ios::in);
    if(!fin.is_open()){
        std::cerr << "Error: File not found" << std::endl;
        return 1;
    }
    while(1){
        std::getline(fin, line, ';'); 
        Lexer lexer(line);
        std::vector<Token> tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.setOutputFile(std::string(outputPath));
        try {
            parser.parse();
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        if(fin.eof()){
            fin.close(); 
            return 0;
        }
    }
}