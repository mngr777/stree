#include <stree/node/packed/manager.hpp>

namespace stree {

#define STREE_TMP_MEMBER_IMPL(_Node, _type, _member)                    \
    template<>                                                          \
    Index NodeManager::alloc<_Node, _type>() {                          \
        return _member.alloc();                                         \
    }                                                                   \
    template<>                                                          \
    _Node& NodeManager::get<_Node, _type>(Index index) {                \
        return _member.get(index);                                      \
    }                                                                   \
    template<>                                                          \
    const _Node& NodeManager::get<_Node, _type>(Index index) const {    \
        return _member.get(index);                                      \
    }                                                                   \
    template<>                                                          \
    void NodeManager::free<_Node, _type>(Index index) {                 \
        _member.free(index);                                            \
    }

#define STREE_TMP_MEMBER_FUN_IMPL(_arity)                               \
    STREE_TMP_MEMBER_IMPL(FunctionNode<_arity>, TypeFunction, fun ## _arity);

STREE_TMP_MEMBER_IMPL(Position, TypePositional, pos)
STREE_TMP_MEMBER_IMPL(Value, TypeConst, val)
STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MEMBER_FUN_IMPL)

#undef STREE_TMP_MEMBER_FUN_IMPL
#undef STREE_TMP_MEMBER_IMPL

} // namespace stree
