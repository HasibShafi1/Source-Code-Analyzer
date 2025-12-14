#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <map>

struct Symbol {
    std::string name;
    std::string type;
    int line;
    int scopeLevel; // 0 for global
};

class SymbolTable {
public:
    SymbolTable();
    
    // Core operations
    bool add(std::string name, std::string type, int line);
    Symbol* lookup(std::string name);
    
    // Scope management
    void enterScope();
    void exitScope();
    
    // For reporting
    std::vector<Symbol> getAllSymbols();

private:
    std::vector<Symbol> allSymbols; // For final report
    // Map name to vector of symbols (shadowing support)
    std::map<std::string, std::vector<Symbol>> activeSymbols;
    int currentScope;
};

#endif // SYMBOL_TABLE_H
