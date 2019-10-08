#include <stree/node/packed/id.hpp>
#include <stree/node/packed/manager.hpp>

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

Id::Id(Type type, Arity arity, Index index) {
    set_type(type);
    set_arity(arity);
    set_index(index);
}

bool Id::operator==(const Id& other) const {
    return data_ == other.data_;
}

bool Id::operator!=(const Id& other) const {
    return !(*this == other);
}

std::size_t Id::hash() const {
    return data_;
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

} // namespace stree
