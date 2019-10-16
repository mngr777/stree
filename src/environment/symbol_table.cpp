#include <stree/environment/symbol_table.hpp>
#include <stdexcept>
#include <cassert>

namespace stree {

void SymbolTable::add(const SymbolPtr& symbol) {
    assert(symbol);
    add_to_map(symbol);
    add_to_list(symbol);
    add_to_term_list(symbol);
    add_to_arity_list(symbol);
    add_to_type_map(symbol);
}

const SymbolPtr& SymbolTable::operator[](std::size_t n) const {
    try {
        return list_.at(n);
    } catch (std::out_of_range& e) {
        throw std::out_of_range(
            std::string("Symbol with index ")
            + std::to_string(static_cast<unsigned long>(n))
            + " does not exist");
    }
}

std::size_t SymbolTable::size() const {
    return list_.size();
}

const SymbolPtr& SymbolTable::by_name(const std::string& name) const {
    try {
        return map_.at(name);
    } catch (std::out_of_range& e) {
        throw std::out_of_range(
            std::string("Symbol `") + "' not found");
    }
}

// NOTE: throws if arity list not found
// alternative: return reference to dummy SymbolPtrList
const SymbolPtrList& SymbolTable::list_by_arity(const Arity arity) const {
    try {
        return list_arity_map_.at(arity);
    } catch (std::out_of_range& e) {
        throw std::out_of_range(
            std::string("No symbols with arity ")
            + std::to_string(static_cast<unsigned>(arity)) + " found");
    }
}

const SymbolPtr& SymbolTable::by_position(Position position) const {
    try {
        return positional_map_.at(position);
    } catch (std::out_of_range& e) {
        throw std::out_of_range(
            std::string("Positional with position ")
            + std::to_string(static_cast<unsigned>(position))
            + " not found");
    }
}

const SymbolPtr& SymbolTable::by_fid(FunctionIndex fid) const {
    try {
        return function_map_.at(fid);
    } catch (std::out_of_range& e) {
        throw std::out_of_range(
            std::string("Function with fid ")
            + std::to_string(static_cast<unsigned>(fid))
            + " not found");
    }
}

const SymbolPtr& SymbolTable::by_sfid(SelectFunctionIndex sfid) const {
    try {
        return select_function_map_.at(sfid);
    } catch (std::out_of_range& e) {
        throw std::out_of_range(
            std::string("Select function with sfid ")
            + std::to_string(static_cast<unsigned>(sfid))
            + " not found");
    }
}


void SymbolTable::add_to_map(const SymbolPtr& symbol) {
    if (map_.count(symbol->name()) > 0)
        throw std::out_of_range(
            std::string("Symbol `") + symbol->name() + "' already exists");
    map_[symbol->name()] = symbol;
}

void SymbolTable::add_to_list(const SymbolPtr& symbol) {
    list_.push_back(symbol);
}

void SymbolTable::add_to_term_list(const SymbolPtr& symbol) {
    if (symbol->is_terminal()) {
        terminals_.push_back(symbol);
    } else {
        nonterminals_.push_back(symbol);
    }
}

void SymbolTable::add_to_arity_list(const SymbolPtr& symbol) {
    list_arity_map_[symbol->arity()].push_back(symbol);
}

void SymbolTable::add_to_type_map(const SymbolPtr& symbol) {
    switch (symbol->type()) {
        case TypeConst:
            // do nothing
            break;
        case TypePositional:
            add_to_positional_map(symbol);
            break;
        case TypeFunction:
            add_to_function_map(symbol);
            break;
        case TypeSelect:
            add_to_select_function_map(symbol);
            break;
    }
}

// NOTE: aliases should probably be allowed in future (for other types too?)
void SymbolTable::add_to_positional_map(const SymbolPtr& symbol) {
    assert(symbol->type() == TypePositional);
    if (positional_map_.count(symbol->position()) > 0)
        throw std::out_of_range(
            std::string("Positional with position ")
            + std::to_string(static_cast<unsigned>(symbol->position()))
            + " already exists");
    positional_map_[symbol->position()] = symbol;
}

void SymbolTable::add_to_function_map(const SymbolPtr& symbol) {
    assert(symbol->type() == TypeFunction);
    if (function_map_.count(symbol->fid()) > 0)
        throw std::out_of_range(
            std::string("Function with fid ")
            + std::to_string(static_cast<unsigned>(symbol->fid()))
            + " already exists");
    function_map_[symbol->fid()] = symbol;
}

void SymbolTable::add_to_select_function_map(const SymbolPtr& symbol) {
    assert(symbol->type() == TypeSelect);
    if (select_function_map_.count(symbol->sfid()) > 0)
        throw std::out_of_range(
            std::string("Select function with sfid ")
            + std::to_string(static_cast<unsigned>(symbol->sfid()))
            + " already exists");
    select_function_map_[symbol->sfid()] = symbol;
}

}
