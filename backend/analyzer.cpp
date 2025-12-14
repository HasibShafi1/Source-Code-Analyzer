#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"

// Helper to escape JSON strings
std::string jsonEscape(const std::string& s) {
    std::ostringstream o;
    for (char c : s) {
        switch (c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    o << "\\u" << std::hex << (int)c; // Simple enough for now
                } else {
                    o << c;
                }
        }
    }
    return o.str();
}

int main() {
    // Read source code from stdin
    std::string source;
    std::string line;
    while (std::getline(std::cin, line)) {
        source += line + "\n";
    }

    // 1. Lexical Analysis
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Syntax & Semantic Analysis
    SymbolTable symTable;
    Parser parser(tokens, symTable);
    parser.parse();

    std::vector<std::string> syntaxErrors = parser.getSyntaxErrors();
    std::vector<std::string> semanticErrors = parser.getSemanticErrors();
    std::vector<Symbol> symbols = symTable.getAllSymbols();

    // 3. Generate JSON Output
    std::cout << "{\n";
    
    // Total Tokens
    std::cout << "  \"totalTokens\": " << tokens.size() << ",\n";
    
    // Tokens
    std::cout << "  \"tokens\": [\n";
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "    { \"type\": \"" << tokens[i].typeToString() << "\", "
                  << "\"value\": \"" << jsonEscape(tokens[i].value) << "\", "
                  << "\"line\": " << tokens[i].line << " }";
        if (i < tokens.size() - 1) std::cout << ",";
        std::cout << "\n";
    }
    std::cout << "  ],\n";

    // Syntax Errors
    std::cout << "  \"syntaxErrors\": [\n";
    for (size_t i = 0; i < syntaxErrors.size(); ++i) {
        std::cout << "    \"" << jsonEscape(syntaxErrors[i]) << "\"";
        if (i < syntaxErrors.size() - 1) std::cout << ",";
        std::cout << "\n";
    }
    std::cout << "  ],\n";

    // Semantic Errors
    std::cout << "  \"semanticErrors\": [\n";
    for (size_t i = 0; i < semanticErrors.size(); ++i) {
        std::cout << "    \"" << jsonEscape(semanticErrors[i]) << "\"";
        if (i < semanticErrors.size() - 1) std::cout << ",";
        std::cout << "\n";
    }
    std::cout << "  ],\n";

    // Symbol Table
    std::cout << "  \"symbolTable\": [\n";
    for (size_t i = 0; i < symbols.size(); ++i) {
        std::cout << "    { \"name\": \"" << jsonEscape(symbols[i].name) << "\", "
                  << "\"type\": \"" << jsonEscape(symbols[i].type) << "\", "
                  << "\"line\": " << symbols[i].line << " }";
        if (i < symbols.size() - 1) std::cout << ",";
        std::cout << "\n";
    }
    std::cout << "  ]\n";

    std::cout << "}\n";

    return 0;
}
