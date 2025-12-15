#include "lexer.h"
#include <cctype>
#include <unordered_set>

Lexer::Lexer(std::string source) : source(source), pos(0), line(1) {
    length = source.length();
}

char Lexer::peek(int offset) {
    if (pos + offset >= length) return '\0';
    return source[pos + offset];
}

char Lexer::advance() {
    if (pos >= length) return '\0';
    return source[pos++];
}

bool Lexer::isAtEnd() {
    return pos >= length;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else if (c == '\n') {
            line++;
            advance();
        } else {
            break;
        }
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        tokens.push_back(scanToken());
    }
    tokens.push_back({END_OF_FILE, "", line});
    return tokens;
}

Token Lexer::scanToken() {
    char c = peek();

    // Identifiers and Keywords
    if (isalpha(c) || c == '_') {
        return identifier();
    }

    // Numbers
    if (isdigit(c)) {
        return number();
    }

    // Operators and Separators
    advance(); // Consume the character
    
    switch (c) {
        case '+': return {OPERATOR, "+", line};
        case '-': return {OPERATOR, "-", line};
        case '*': return {OPERATOR, "*", line};
        case '/': 
            if (peek() == '/') {
                // Single-line comment
                advance(); // consume second /
                std::string content = "//";
                while (peek() != '\n' && !isAtEnd()) {
                    content += advance();
                }
                // Don't consume the newline here, let skipWhitespace handle it in next loop
                return {COMMENT, content, line};
            } else if (peek() == '*') {
                // Multi-line comment
                std::string content = "/*";
                advance(); // consume *
                while (!isAtEnd()) {
                    if (peek() == '*' && peek(1) == '/') {
                        advance(); advance(); // consume */
                        content += "*/";
                        break;
                    }
                    if (peek() == '\n') line++;
                    content += advance();
                }
                return {COMMENT, content, line};
            }
            return {OPERATOR, "/", line};
        case '(': return {SEPARATOR, "(", line};
        case ')': return {SEPARATOR, ")", line};
        case '{': return {SEPARATOR, "{", line};
        case '}': return {SEPARATOR, "}", line};
        case ';': return {SEPARATOR, ";", line};
        case ',': return {SEPARATOR, ",", line};
        case '=': 
            if (peek() == '=') {
                advance();
                return {OPERATOR, "==", line};
            }
            return {OPERATOR, "=", line};
        case '<': return {OPERATOR, "<", line};
        case '>': return {OPERATOR, ">", line};
    }

    return {UNKNOWN, std::string(1, c), line};
}

Token Lexer::identifier() {
    std::string text = "";
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
        text += advance();
    }

    TokenType type = IDENTIFIER;
    static const std::unordered_set<std::string> keywords = {
        "int", "float", "if", "else", "while", "return"
    };

    if (keywords.find(text) != keywords.end()) {
        type = KEYWORD;
    }

    return {type, text, line};
}

Token Lexer::number() {
    std::string text = "";
    while (!isAtEnd() && isdigit(peek())) {
        text += advance();
    }
    // Allow float
    if (peek() == '.' && isdigit(peek(1))) {
        text += advance(); // Consume .
        while (!isAtEnd() && isdigit(peek())) {
            text += advance();
        }
    }
    return {NUMBER, text, line};
}
