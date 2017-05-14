#include <stree/tree.hpp>
#include <cassert>

std::ostream& operator<<(std::ostream& os, const stree::Id& id) {
    os << '(';
    if (!id.empty()) {
        os << "type: " << type_to_string(id.type())
           << ", arity: " << static_cast<unsigned>(id.arity())
           << ", index: " << id.index();
    } else {
        os << "empty";
    }
    return os << ')';
}

std::ostream& operator<<(std::ostream& os, const stree::FunctionId& fid) {
    os << '(';
    if (!fid.empty()) {
        os << "arity: " << static_cast<unsigned>(fid.arity())
           << ", index: " << fid.index();
    } else {
        os << "empty";
    }
    return os << ')';
}

namespace {

template<typename D, typename V>
V get(D data, D mask, std::uint8_t offset) {
    return (data & mask) >> offset;
}

template<typename D, typename V>
void set(D& data, D mask, std::uint8_t offset, V value) {
    data = (data & ~mask)
        | ((static_cast<D>(value) << offset)
           & mask);
}

} // namespace


namespace stree {

const char* type_to_string(Type type) {
    switch (type) {
        case TypeConst:
            return "con";
        case TypePositional:
            return "pos";
        case TypeFunction:
            return "fun";
        case TypeSelect:
            return "sel";
    }
    assert(false);
}

Id::Id(Type type, Arity arity, Index index) {
    set_type(type);
    set_arity(arity);
    set_index(index);
}

Type Id::type() const {
    return type_id_to_type(
        get<Index, TypeId>(data_, TypeMask, IndexWidth + ArityWidth));
}

void Id::set_type(Type type) {
    set(data_, TypeMask, IndexWidth + ArityWidth, type_to_type_id(type));
}

Arity Id::arity() const {
    return get<Index, Arity>(data_, ArityMask, IndexWidth);
}

void Id::set_arity(Arity arity) {
    set(data_, ArityMask, IndexWidth, arity);
}

Index Id::index() const {
    return get<Index, Index>(data_, IndexMask, 0);
}

void Id::set_index(Index index) {
    set(data_, IndexMask, 0, index);
}


FunctionId::FunctionId(Arity arity, FunctionIndex index) {
    set_arity(arity);
    set_index(index);
}

Arity FunctionId::arity() const {
    return get<FunctionIndex, Arity>(data_, ArityMask, FunctionIndexWidth);
}

void FunctionId::set_arity(Arity arity) {
    set(data_, ArityMask, FunctionIndexWidth, arity);
}

Index FunctionId::index() const {
    return get<FunctionIndex, FunctionIndex>(data_, FunctionIndexMask, 0);
}

void FunctionId::set_index(FunctionIndex index) {
    return set(data_, FunctionIndexMask, 0, index);
}

} // namespace stree
