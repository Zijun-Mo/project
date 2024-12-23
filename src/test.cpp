#include <bits/stdc++.h>
#include "Lexer.hpp"
#include "parser.hpp"

std::string line; 
int main(){
    while(1){
        std::getline(std::cin, line, ';'); 
        Lexer lexer(line);
        std::vector<Token> tokens = lexer.tokenize();
        for(auto& token : tokens){
            std::cout << token.type << " " << token.value << std::endl;
        }
        Parser parser(tokens);
        parser.setOutputFile("output.csv");
        parser.parse();
    }
    

    return 0;
}