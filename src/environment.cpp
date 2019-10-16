#include <stree/environment.hpp>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace stree {

// TODO: check if arity is valid when adding functions/selects

void Environment::add_function(
    const std::string& name,
    Arity arity,
    Function function)
{
    // Add function
    FunctionIndex fid = functions_.size();
    functions_.push_back(function);

    // Add symbol
    SymbolPtr symbol = make_symbol(name, TypeFunction);
    symbol->set_arity(arity);
    symbol->set_fid(fid);
    add_symbol(symbol);
}

Function Environment::function(FunctionIndex fid) const {
    return functions_.at(fid);
}

void Environment::add_select_function(
        const std::string& name,
        Arity arity,
        Arity sf_arity,
        SelectFunction select_function)
{
    // Add select function
    SelectFunctionIndex sfid = select_functions_.size();
    select_functions_.push_back(select_function);

    // Add symbol
    SymbolPtr symbol = make_symbol(name, TypeSelect);
    symbol->set_arity(arity);
    symbol->set_sfid(sfid);
    symbol->set_sf_arity(sf_arity);
    add_symbol(symbol);
}

SelectFunction Environment::select_function(SelectFunctionIndex sfid) const {
    return select_functions_.at(sfid);
}

// TODO: remove
Arity Environment::select_function_cond_arity(SelectFunctionIndex sfid) const {
    return symbol_table_.by_sfid(sfid)->sf_arity();
}


void Environment::add_positional(const std::string& name, Position position) {
    SymbolPtr symbol = make_symbol(name, TypePositional);
    symbol->set_position(position);
    add_symbol(symbol);
}

void Environment::add_constant(const std::string& name, const Value& value) {
    SymbolPtr symbol = make_symbol(name, TypeConst);
    symbol->set_value(value);
    add_symbol(symbol);
}

void Environment::add_constant(const std::string& name) {
    Value value{};
    add_constant(name, value);
}

const SymbolPtr& Environment::symbol(const std::string& name) const {
    return symbol_table_.by_name(name);
}

const SymbolPtr& Environment::symbol(const Id& id) const {
    switch (id.type()) {
        case TypeConst:
            throw std::invalid_argument("Cannot find symbol for constant");

        case TypePositional: {
            Position position = id::position(node_manager_, id);
            return symbol_table_.by_position(position);
        }
        case TypeFunction: {
            FunctionIndex fid = id::fid(node_manager_, id);
            return symbol_table_.by_fid(fid);
        }
        case TypeSelect: {
            SelectFunctionIndex sfid = id::sfid(node_manager_, id);
            return symbol_table_.by_sfid(sfid);
        }
    }
    assert(false);
}

const SymbolPtr& Environment::symbol(unsigned n) const {
    return symbol_table_[n];
}

unsigned Environment::symbol_num() const {
    return symbol_table_.size();
}


unsigned Environment::symbol_by_arity_num(Arity arity) const {
    return symbol_table_.list_by_arity(arity).size();
}

const SymbolPtr& Environment::symbol_by_arity(Arity arity, unsigned n) const {
    return symbol_table_.list_by_arity(arity).at(n);
}

unsigned Environment::terminal_num() const {
    return symbol_table_.terminals().size();
}

const SymbolPtr& Environment::terminal(unsigned n) const {
    return symbol_table_.terminals().at(n);
}

unsigned Environment::nonterminal_num() const {
    return symbol_table_.nonterminals().size();
}

const SymbolPtr& Environment::nonterminal(unsigned n) const {
    return symbol_table_.nonterminals().at(n);
}

// TODO: rename to allow for using Value = std::string
Id Environment::make_id(const std::string& name) {
    return make_id(symbol(name));
}

Id Environment::make_id(const SymbolPtr& symbol) {
    Id id = id::make(node_manager_, symbol->type(), symbol->arity());
    switch (symbol->type()) {
        case TypeConst:
            id::set_value(node_manager_, id, symbol->value());
            break;
        case TypePositional:
            id::set_position(node_manager_, id, symbol->position());
            break;
        case TypeFunction:
            id::set_fid(node_manager_, id, symbol->fid());
            break;
        case TypeSelect:
            id::set_sfid(node_manager_, id, symbol->sfid());
            break;
    }
    return id;
}

Id Environment::make_id(const Value& value) {
    Id id = id::make(node_manager_, TypeConst, 0);
    id::set_value(node_manager_, id, value);
    return id;
}

void Environment::add_symbol(const SymbolPtr& symbol) {
    symbol_table_.add(symbol);
}

} // namespace stree {
