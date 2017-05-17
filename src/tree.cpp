#include <stree/tree.hpp>
#include <cassert>

std::ostream& operator<<(std::ostream& os, const stree::Id id) {
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


// Node manager

#define STREE_TMP_MEMBER_IMPL(_Type, _member)           \
    template<>                                          \
    Index NodeManager::alloc_node<_Type>() {            \
        return _member.alloc();                         \
    }                                                   \
    template<>                                          \
    _Type& NodeManager::get_node<_Type>(Index index) {  \
        return _member.get(index);                      \
    }                                                   \
    template<>                                          \
    void NodeManager::free_node<_Type>(Index index) {   \
        _member.free(index);                            \
    }

#define STREE_TMP_MEMBER_FUN_IMPL(_arity)                       \
    STREE_TMP_MEMBER_IMPL(FunctionNode<_arity>, fun ## _arity);

STREE_TMP_MEMBER_IMPL(Position, pos)
STREE_TMP_MEMBER_IMPL(Value, val)
STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MEMBER_FUN_IMPL)

#undef STREE_TMP_MEMBER_FUN_IMPL
#undef STREE_TMP_MEMBER_IMPL


#define STREE_TMP_MAKE_FUN_ARITY_CASE(_arity)       \
    else if (arity == _arity) {                     \
        index = alloc_node<FunctionNode<_arity>>(); \
    }

Id NodeManager::make(Type type, Arity arity) {
    Index index = Id::NoIndex;
    switch (type) {
        case TypeConst:
            index = alloc_node<Value>();
            break;
        case TypePositional:
            index = alloc_node<Position>();
            break;
        case TypeFunction:
            if (false) {}
            STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MAKE_FUN_ARITY_CASE)
                break;
        case TypeSelect:
            // TODO
            break;
    }
    return Id(type, arity, index);
}
#undef STREE_TMP_MAKE_FUN_ARITY_CASE


#define SMTREE_TMP_DESTROY_FUN_ARITY_CASE(_arity)       \
    else if(id.arity() == _arity) {                     \
        free_node<FunctionNode<_arity>>(id.index());    \
    }

void NodeManager::destroy(Id id) {
    switch (id.type()) {
        case TypeConst:
            free_node<Value>(id.index());
            break;
        case TypePositional:
            free_node<Position>(id.index());
            break;
        case TypeFunction:
            if (false) {}
            STREE_FOR_EACH_FUN_ARITY(SMTREE_TMP_DESTROY_FUN_ARITY_CASE)
            else { assert(false && "Invalid arity"); }
            break;
        case TypeSelect:
            // TODO
            break;
    }
}
#undef SMTREE_TMP_DESTROY_FUN_ARITY_CASE


#define SMTREE_TMP_ARGUMENT_FUN_ARITY_CASE(_arity)                      \
    else if (id.arity() == _arity) {                                     \
        return nm.get_node<FunctionNode<_arity>>(id.index()).argument(n); \
    }

Id nth_argument(NodeManager& nm, Id id, Arity n) {
    switch (id.type()) {
        case TypeConst:
        case TypePositional:
            return Id(); // empty
        case TypeFunction:
            if (false) {}
            STREE_FOR_EACH_FUN_ARITY(SMTREE_TMP_ARGUMENT_FUN_ARITY_CASE)
            else { assert(false && "Invalid arity"); }
        case TypeSelect:
            // TODO
            return Id();
    }
    assert(false);
}
#undef SMTREE_TMP_ARGUMENT_FUN_ARITY_CASE


#define SMTREE_TMP_SET_ARGUMENT_FUN_ARITY_CASE(_arity)                  \
    else if (id.arity() == _arity) {                                    \
        nm.get_node<FunctionNode<_arity>>(id.index()).set_argument(n, argument_id); \
    }

void set_nth_argument(NodeManager& nm, Id id, Arity n, Id argument_id) {
    switch (id.type()) {
        case TypeConst:
        case TypePositional:
            assert(false && "Cannot set argument to leaf node");
            break;
        case TypeFunction:
            if (false) {}
            STREE_FOR_EACH_FUN_ARITY(SMTREE_TMP_SET_ARGUMENT_FUN_ARITY_CASE)
            else { assert(false && "Invalid arity"); }
        case TypeSelect:
            // TODO
            break;
    }
}
#undef SMTREE_TMP_SET_ARGUMENT_FUN_ARITY_CASE


Id copy(NodeManager& nm, Id id) {
    return nm.make(id.type(), id.arity());
}

Id copy_subtree(NodeManager& nm, Id root) {
    Id root_copy = copy(nm, root);
    for (Arity n = 0; n < root.arity(); ++n)
        set_nth_argument(
            nm, root, n,
            copy_subtree(
                nm,
                nth_argument(nm, root, n)));
    return root_copy;
}

void destroy_subtree(NodeManager& nm, Id root) {
    for (Arity n = 0; n < root.arity(); ++ n)
        destroy_subtree(nm, nth_argument(nm, root, n));
    nm.destroy(root);
}

} // namespace stree
