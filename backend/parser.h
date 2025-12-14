#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "token.h"
#include "symbol_table.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens, SymbolTable& symTable);
    void parse();
    
    std::vector<std::string> getSyntaxErrors() const;
    std::vector<std::string> getSemanticErrors() const;

private:
    const std::vector<Token>& tokens;
    SymbolTable& symTable;
    int pos;
    
    std::vector<std::string> syntaxErrors;
    std::vector<std::string> semanticErrors;

    Token peek(int offset = 0);
    Token advance();
    bool match(TokenType type, std::string value = "");
    bool check(TokenType type, std::string value = "");
    Token consume(TokenType type, std::string errorMessage);
    Token consume(TokenType type, std::string value, std::string errorMessage);

    void program();
    void statement();
    void variableDeclaration();
    void assignment();
    void ifStatement();
    void whileStatement();
    void block();
    void expression();
    void term();
    void factor();
    
    void error(std::string message);
    void semanticError(std::string message);
    void synchronize();
};

#endif // PARSER_H
