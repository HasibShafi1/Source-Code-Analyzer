#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    SEPARATOR,
    COMMENT,
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;

    std::string typeToString() const {
        switch (type) {
            case KEYWORD: return "KEYWORD";
            case IDENTIFIER: return "IDENTIFIER";
            case NUMBER: return "NUMBER";
            case OPERATOR: return "OPERATOR";
            case SEPARATOR: return "SEPARATOR";
            case COMMENT: return "COMMENT";
            case UNKNOWN: return "UNKNOWN";
            case END_OF_FILE: return "EOF";
            default: return "UNKNOWN";
        }
    }
};

#endif // TOKEN_H
