#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "token.h"

class Lexer {
public:
    Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source;
    int pos;
    int length;
    int line;
    
    char peek(int offset = 0);
    char advance();
    bool isAtEnd();
    void skipWhitespace();
    
    Token scanToken();
    Token identifier();
    Token number();
};

#endif // LEXER_H
