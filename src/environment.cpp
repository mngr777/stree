#include <stree/environment.hpp>
#include <cassert>
#include <stdexcept>

namespace stree {

Position Symbol::position() const {
    assert(type_ == TypePositional);
    return data_.positional.position;
}

void Symbol::set_position(Position position) {
    assert(type_ == TypePositional);
    data_.positional.position = position;
}

Arity Symbol::arity() const {
    assert(type_ == TypeFunction);
    return data_.function.arity;
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

Symbol* Environment::symbol_by_name(const std::string& name) {
    auto it = symbol_map_.find(name);
    return (it != symbol_map_.end()) ? &(*it).second : nullptr;
}

void Environment::add_symbol(const std::string& name, Symbol symbol) {
    if (symbol_map_.find(name) != symbol_map_.end())
        throw std::invalid_argument(
            std::string("Symbol already exists: ") + name);
    symbol_map_.emplace(name, symbol);
}

} // namespace stree {
