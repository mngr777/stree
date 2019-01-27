#include <stree/environment.hpp>
#include <cassert>
#include <stdexcept>
#include <utility>

namespace stree {

bool Symbol::operator==(const Symbol& other) const {
    return name() == other.name();
}

bool Symbol::operator!=(const Symbol& other) const {
    return !(*this == other);
}

Arity Symbol::arity() const {
    switch (type_) {
        case TypeConst:
        case TypePositional:
            return 0;
        case TypeFunction:
            return data_.function.arity;
        case TypeSelect:
            return data_.select.arity;
    }
    assert(false && "Undefined type");
}

Value Symbol::value() const {
    assert(type_ == TypeConst);
    return data_.constant.value;
}

void Symbol::set_value(Value value) {
    assert(type_ == TypeConst);
    data_.constant.value = std::move(value);
}

Position Symbol::position() const {
    assert(type_ == TypePositional);
    return data_.positional.position;
}

void Symbol::set_position(Position position) {
    assert(type_ == TypePositional);
    data_.positional.position = position;
}

void Symbol::set_arity(Arity arity) {
    assert(type_ == TypeFunction);
    data_.function.arity = arity;
}

FunctionIndex Symbol::fid() const {
    assert(type_ == TypeFunction);
    return data_.function.fid;
}

void Symbol::set_fid(FunctionIndex fid) {
    assert(type_ == TypeFunction);
    data_.function.fid = fid;
}

SelectFunctionIndex Symbol::sfid() const {
    assert(type_ == TypeSelect);
    return data_.select.sfid;
}

void Symbol::set_sfid(SelectFunctionIndex sfid) {
    assert(type_ == TypeSelect);
    data_.select.sfid = sfid;
}


void Environment::add_function(
    const std::string& name,
    Arity arity,
    Function function)
{
    functions_.push_back(function);
    FunctionIndex fid = functions_.size() - 1;
    Symbol symbol(name, TypeFunction);
    symbol.set_arity(arity);
    symbol.set_fid(fid);
    add_symbol(symbol);
}

Function Environment::function(FunctionIndex fid) const {
    assert(fid < functions_.size());
    return functions_[fid];
}

void Environment::add_positional(const std::string& name, Position position) {
    Symbol symbol(name, TypePositional);
    symbol.set_position(position);
    add_symbol(symbol);
}

const Symbol* Environment::symbol(const std::string& name) const {
    auto it = symbol_map_.find(name);
    return (it != symbol_map_.end()) ? &(*it).second : nullptr;
}

// TODO: create lookup table
const Symbol* Environment::symbol(const Id& id) const {
    for (const auto& item : symbol_map_) {
        const Symbol& symbol = item.second;
        if (id.type() == symbol.type()) {
            switch(id.type()) {
                case TypeConst:
                    break; // never resolve constant values
                case TypePositional:
                    if (id::position(node_manager_, id) == symbol.position())
                        return &symbol;
                    break;
                case TypeFunction:
                    if (id::fid(node_manager_, id) == symbol.fid())
                        return &symbol;
                    break;
                case TypeSelect:
                    // TODO
                    break;
            }
        }
    }
    return nullptr;
}

unsigned Environment::symbol_num() const {
    return symbols_.size();
}

const Symbol* Environment::symbol(unsigned n) const {
    return symbols_[n];
}


unsigned Environment::symbol_by_arity_num(Arity arity) const {
    auto it = symbol_list_arity_map_.find(arity);
    return it != symbol_list_arity_map_.end()
        ? it->second.size()
        : 0;
}

const Symbol* Environment::symbol_by_arity(Arity arity, unsigned n) const {
    auto it = symbol_list_arity_map_.find(arity);
    return it != symbol_list_arity_map_.end()
        ? it->second.at(n)
        : nullptr;
}

unsigned Environment::terminal_num() const {
    return symbol_by_arity_num(0);
}

const Symbol* Environment::terminal(unsigned n) const {
    return symbol_by_arity(0, n);
}

unsigned Environment::nonterminal_num() const {
    return nonterminals_.size();
}

const Symbol* Environment::nonterminal(unsigned n) const {
    return nonterminals_.at(n);
}

Id Environment::make_id(const Symbol* symbol) {
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
            // TODO
            // id::set_sfid(node_manager_, id, symbol->sfid());
            break;
    }
    return id;
}

void Environment::add_symbol(Symbol smb) {
    if (symbol_map_.find(smb.name()) != symbol_map_.end())
        throw std::invalid_argument(
            std::string("Symbol already exists: ") + smb.name());
    symbol_map_.emplace(smb.name(), smb);

    const Symbol* smb_ptr = symbol(smb.name());
    assert(smb_ptr && "Cannot find added symbol");
    symbols_.push_back(smb_ptr);
    add_symbol_to_arity_lists(smb_ptr);
}

void Environment::add_symbol_to_arity_lists(const Symbol* symbol) {
    assert(symbol);
    Arity arity = symbol->arity();

    // add to nonterminal list
    if (arity > 0) {
        nonterminals_.push_back(symbol);
    }

    // add to lists by arity
    symbol_list_arity_map_[arity].push_back(symbol);
}

} // namespace stree {
