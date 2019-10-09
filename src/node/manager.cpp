#include <stree/node/manager.hpp>

namespace stree {

#define STREE_TMP_MEMBER_IMPL(_Node, _type, _member)                    \
    template<>                                                          \
    Id::Index NodeManager::alloc<_Node, _type>() {                      \
        return _member.alloc();                                         \
    }                                                                   \
    template<>                                                          \
    _Node& NodeManager::get<_Node, _type>(Id::Index index) {            \
        return _member.get(index);                                      \
    }                                                                   \
    template<>                                                          \
    const _Node& NodeManager::get<_Node, _type>(Id::Index index) const { \
        return _member.get(index);                                      \
    }                                                                   \
    template<>                                                          \
    void NodeManager::free<_Node, _type>(Id::Index index) {             \
        _member.free(index);                                            \
    }

#define STREE_TMP_MEMBER_FUN_IMPL(_arity)                               \
    STREE_TMP_MEMBER_IMPL(FunctionNode<_arity>, TypeFunction, fun ## _arity);

#define STREE_TMP_MEMBER_SELECT_IMPL(_arity)                            \
    STREE_TMP_MEMBER_IMPL(SelectNode<_arity>, TypeSelect, select ## _arity);

STREE_TMP_MEMBER_IMPL(PositionalNode, TypePositional, pos)
STREE_TMP_MEMBER_IMPL(ConstNode, TypeConst, val)
STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MEMBER_FUN_IMPL)
STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_MEMBER_SELECT_IMPL)

#undef STREE_TMP_MEMBER_FUN_IMPL
#undef STREE_TMP_MEMBER_SELECT_IMPL
#undef STREE_TMP_MEMBER_IMPL

} // namespace stree
