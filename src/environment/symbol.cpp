#include <stree/environment/symbol.hpp>
#include <cassert>

namespace stree {

SymbolPtr make_symbol(std::string name, Type type) {
    return std::make_shared<Symbol>(std::move(name), type);
}

bool Symbol::operator==(const Symbol& other) const {
    // TODO: compare by type and value
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
    assert(type_ == TypeFunction || type_ == TypeSelect);
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

Arity Symbol::sf_arity() const {
    assert(type_ == TypeSelect);
    return data_.select.sf_arity;
}

void Symbol::set_sf_arity(Arity arity) {
    assert(type_ == TypeSelect);
    data_.select.sf_arity = arity;
}

}
