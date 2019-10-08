#include <cassert>
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
namespace id {


#define STREE_TMP_MAKE_FUN_ARITY_CASE(_arity)                   \
    else if (arity == _arity) {                                 \
        index = nm.alloc<FunctionNode<_arity>, TypeFunction>(); \
    }

#define STREE_TMP_MAKE_SELECT_ARITY_CASE(_arity)            \
    else if (arity == _arity) {                             \
        index = nm.alloc<SelectNode<_arity>, TypeSelect>(); \
    }

Id make(NodeManager& nm, Type type, Arity arity) {
    Id::Index index = Id::NoIndex;
    switch (type) {
    case TypeConst:
        index = nm.alloc<Value, TypeConst>();
        break;
    case TypePositional:
        index = nm.alloc<Position, TypePositional>();
        break;
    case TypeFunction:
        if (false) {}
        STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MAKE_FUN_ARITY_CASE)
        else { assert(false && "Invalid function arity"); }
        break;
    case TypeSelect:
        if (false) {}
        STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_MAKE_SELECT_ARITY_CASE)
        else { assert(false && "Invalid select arity"); }
        break;
    }
    assert(index != Id::NoIndex);
    return Id(type, arity, index);
}

#undef STREE_TMP_MAKE_FUN_ARITY_CASE
#undef STREE_TMP_MAKE_SELECT_ARITY_CASE


#define STREE_TMP_DESTROY_FUN_ARITY_CASE(_arity)                    \
    else if(id.arity() == _arity) {                                 \
        nm.free<FunctionNode<_arity>, TypeFunction>(id.index());    \
    }

#define STREE_TMP_DESTROY_SELECT_ARITY_CASE(_arity)             \
    else if (id.arity() == _arity) {                            \
        nm.free<SelectNode<_arity>, TypeSelect>(id.index());    \
    }

void destroy(NodeManager& nm, Id& id) {
    if (!id.empty()) {
        switch (id.type()) {
        case TypeConst:
            nm.free<Value, TypeConst>(id.index());
            break;
        case TypePositional:
            nm.free<Position, TypePositional>(id.index());
            break;
        case TypeFunction:
            if (false) {}
            STREE_FOR_EACH_FUN_ARITY(STREE_TMP_DESTROY_FUN_ARITY_CASE)
            else { assert(false && "Invalid function arity"); }
            break;
        case TypeSelect:
            if (false) {}
            STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_DESTROY_SELECT_ARITY_CASE)
            else { assert(false && "Invalid select arity"); }
            break;
        }
        id.reset();
    }
}

#undef STREE_TMP_DESTROY_FUN_ARITY_CASE
#undef STREE_TMP_DESTROY_SELECT_ARITY_CASE

} // namespace id


Id::Id(Type type, Arity arity, Id::Index index) {
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

Id::Index Id::index() const {
    return get<Index, Index>(data_, IndexMask, 0);
}

void Id::set_index(Id::Index index) {
    set(data_, IndexMask, 0, index);
}

} // namespace stree
