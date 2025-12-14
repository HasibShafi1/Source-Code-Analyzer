#include "symbol_table.h"

SymbolTable::SymbolTable() : currentScope(0) {}

bool SymbolTable::add(std::string name, std::string type, int line) {
    // Check for duplicate in current scope
    if (activeSymbols.count(name)) {
        for (const auto& sym : activeSymbols[name]) {
            if (sym.scopeLevel == currentScope) {
                return false; // Duplicate declaration
            }
        }
    }

    Symbol sym = {name, type, line, currentScope};
    activeSymbols[name].push_back(sym);
    allSymbols.push_back(sym);
    return true;
}

Symbol* SymbolTable::lookup(std::string name) {
    if (activeSymbols.count(name) && !activeSymbols[name].empty()) {
        // Return validity check: simply return the most recent one (shadowing)
        return &activeSymbols[name].back();
    }
    return nullptr;
}

void SymbolTable::enterScope() {
    currentScope++;
}

void SymbolTable::exitScope() {
    // Remove symbols belonging to the current scope
    std::vector<std::string> toRemove;
    
    for (auto& pair : activeSymbols) {
        auto& syms = pair.second;
        while (!syms.empty() && syms.back().scopeLevel == currentScope) {
            syms.pop_back();
        }
        if (syms.empty()) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& name : toRemove) {
        activeSymbols.erase(name);
    }

    currentScope--;
}

std::vector<Symbol> SymbolTable::getAllSymbols() {
    return allSymbols;
}
