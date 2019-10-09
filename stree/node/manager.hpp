#ifndef STREE_NODE_MANAGER_HPP_
#define STREE_NODE_MANAGER_HPP_

#include <stree/macros.hpp>
#include <stree/node/impl.hpp>

namespace stree {

#define STREE_TMP_MEMBER_DECL(_Type, _member)   \
    private:                                    \
    NodePool<_Type> _member;                    \

#define STREE_TMP_MEMBER_FUN_DECL(_arity)                       \
    STREE_TMP_MEMBER_DECL(FunctionNode<_arity>, fun ## _arity);

#define STREE_TMP_MEMBER_SELECT_DECL(_arity)                    \
    STREE_TMP_MEMBER_DECL(SelectNode<_arity>, select ## _arity)

class NodeManager {
    friend NodeManagerStats;
public:
    NodeManager() {}
    NodeManager(const NodeManager& other) = delete;
    NodeManager& operator=(const NodeManager& other) = delete;

    template<typename N, Type type>
    Id::Index alloc();

    template<typename N, Type type>
    N& get(Id::Index index);

    template<typename N, Type type>
    const N& get(Id::Index index) const;

    template<typename N, Type type>
    void free(Id::Index index);

    STREE_TMP_MEMBER_DECL(Position, pos)
    STREE_TMP_MEMBER_DECL(Value, val)
    STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MEMBER_FUN_DECL)
    STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_MEMBER_SELECT_DECL)
};

#undef STREE_TMP_MEMBER_FUN_DECL
#undef STREE_TMP_MEMBER_SELECT_DECL
#undef STREE_TMP_MEMBER_DECL

} // namespace stree

#endif
