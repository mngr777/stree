#include <stree/environment.hpp>
#include <cassert>
#include <stdexcept>
#include <utility>

namespace stree {

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
    Symbol symbol(TypeFunction);
    symbol.set_arity(arity);
    symbol.set_fid(fid);
    add_symbol(name, symbol);
}

void Environment::add_positional(const std::string& name, Position position) {
    Symbol symbol(TypePositional);
    symbol.set_position(position);
    add_symbol(name, symbol);
}

const Symbol* Environment::symbol(const std::string& name) const {
    auto it = symbol_map_.find(name);
    return (it != symbol_map_.end()) ? &(*it).second : nullptr;
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

void Environment::add_symbol(const std::string& name, Symbol symbol) {
    if (symbol_map_.find(name) != symbol_map_.end())
        throw std::invalid_argument(
            std::string("Symbol already exists: ") + name);
    symbol_map_.emplace(name, symbol);
}

} // namespace stree {
