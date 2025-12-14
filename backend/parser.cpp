#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens, SymbolTable& symTable) 
    : tokens(tokens), symTable(symTable), pos(0) {}

void Parser::parse() {
    program();
}

std::vector<std::string> Parser::getSyntaxErrors() const {
    return syntaxErrors;
}

std::vector<std::string> Parser::getSemanticErrors() const {
    return semanticErrors;
}

Token Parser::peek(int offset) {
    if (pos + offset >= tokens.size()) return tokens.back(); // Return EOF or last
    return tokens[pos + offset];
}

Token Parser::advance() {
    if (pos < tokens.size()) pos++;
    return tokens[pos - 1];
}

bool Parser::match(TokenType type, std::string value) {
    if (check(type, value)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type, std::string value) {
    if (peek().type == END_OF_FILE) return false;
    if (peek().type != type) return false;
    if (!value.empty() && peek().value != value) return false;
    return true;
}

Token Parser::consume(TokenType type, std::string errorMessage) {
    if (check(type)) return advance();
    error("Line " + std::to_string(peek().line) + " " + errorMessage);
    return peek();
}

Token Parser::consume(TokenType type, std::string value, std::string errorMessage) {
    if (check(type, value)) return advance();
    error("Line " + std::to_string(peek().line) + " " + errorMessage);
    return peek();
}

void Parser::error(std::string message) {
    syntaxErrors.push_back(message);
}

void Parser::semanticError(std::string message) {
    semanticErrors.push_back(message);
}

void Parser::synchronize() {
    advance();
    while (peek().type != END_OF_FILE) {
        if (tokens[pos - 1].value == ";") return;
        std::string v = peek().value;
        if (v == "int" || v == "float" || v == "if" || 
            v == "while" || v == "return") return;
        advance();
    }
}

// --- Grammar Rules ---

void Parser::program() {
    while (peek().type != END_OF_FILE) {
        statement();
    }
}

void Parser::statement() {
    if (match(KEYWORD, "int") || match(KEYWORD, "float")) {
        // Backtrack one token to let declaration handle the type
        pos--; 
        variableDeclaration();
    } else if (match(KEYWORD, "if")) {
        ifStatement();
    } else if (match(KEYWORD, "while")) {
        whileStatement();
    } else if (match(SEPARATOR, "{")) {
        // block handled inside if/while mostly, but standalone block:
        pos--; // backtrack
        block();
    } else if (check(IDENTIFIER)) {
        assignment();
    } else {
        error("Line " + std::to_string(peek().line) + " INVALID statement order or unknown token " + peek().value);
        advance();
    }
}

void Parser::variableDeclaration() {
    Token typeToken = advance(); // int or float
    if (!check(IDENTIFIER)) {
        error("Line " + std::to_string(peek().line) + " Expect variable name.");
        synchronize();
        return;
    }
    Token nameToken = advance(); // We know it's an identifier
    
    if (!symTable.add(nameToken.value, typeToken.value, nameToken.line)) {
        semanticError("Line " + std::to_string(nameToken.line) + " Duplicate declaration of " + nameToken.value);
    }
    
    if (match(OPERATOR, "=")) {
        expression();
    }
    
    consume(SEPARATOR, ";", "Missing semicolon");
}

void Parser::assignment() {
    Token nameToken = consume(IDENTIFIER, "Expect variable name.");
    
    Symbol* sym = symTable.lookup(nameToken.value);
    if (!sym) {
        semanticError("Line " + std::to_string(nameToken.line) + " Variable " + nameToken.value + " used without declaration");
    }
    
    consume(OPERATOR, "=", "Expect '=' after variable.");
    expression();
    consume(SEPARATOR, ";", "Missing semicolon");
}

void Parser::ifStatement() {
    consume(SEPARATOR, "(", "Expect '(' after 'if'.");
    expression();
    consume(SEPARATOR, ")", "Expect ')' after condition.");
    
    if (match(SEPARATOR, "{")) {
        pos--; // backtrack
        block();
    } else {
        statement(); // Single statement
    }
    
    if (match(KEYWORD, "else")) {
        if (match(SEPARATOR, "{")) {
            pos--;
            block();
        } else {
            statement();
        }
    }
}

void Parser::whileStatement() {
    consume(SEPARATOR, "(", "Expect '(' after 'while'.");
    expression();
    consume(SEPARATOR, ")", "Expect ')' after condition.");
    
    if (match(SEPARATOR, "{")) {
        pos--;
        block();
    } else {
        statement();
    }
}

void Parser::block() {
    consume(SEPARATOR, "{", "Expect '{'");
    symTable.enterScope();
    while (!check(SEPARATOR, "}") && !check(END_OF_FILE)) {
        statement();
    }
    consume(SEPARATOR, "}", "Expect '}'");
    symTable.exitScope();
}

void Parser::expression() {
    // Simple expression parser: term (+ term)*
    term();
    while (match(OPERATOR, "+") || match(OPERATOR, "-") || 
           match(OPERATOR, "<") || match(OPERATOR, ">") || match(OPERATOR, "==")) {
        term();
    }
}

void Parser::term() {
    // factor (* factor)*
    factor();
    while (match(OPERATOR, "*") || match(OPERATOR, "/")) {
        factor();
    }
}

void Parser::factor() {
    if (match(NUMBER)) {
        return;
    } else if (match(IDENTIFIER)) {
        Token token = tokens[pos-1];
        if (!symTable.lookup(token.value)) {
            semanticError("Line " + std::to_string(token.line) + " Variable " + token.value + " used without declaration");
        }
    } else if (match(SEPARATOR, "(")) {
        expression();
        consume(SEPARATOR, ")", "Expect ')' after expression.");
    } else {
        // Don't advance here, consume generates error and returns current
        consume(UNKNOWN, "Expect expression.");
    }
}
