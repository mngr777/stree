#ifndef STREE_NODE_IMPL_POINTER_ID_HPP_
#define STREE_NODE_IMPL_POINTER_ID_HPP_

#include <cstddef>
#include <stree/macros.hpp>
#include <stree/types.hpp>
#include <stree/node/functions.hpp>

namespace stree {

class NodeManager;
class NodeBase;
class Tree;

class Id {
    friend void id::destroy(NodeManager& nm, Id& id);
    friend Tree;

public:
    using Index = NodeBase*;
    static constexpr Index NoIndex = nullptr;

    Id() : index_(nullptr) {}
    Id(Index index)
        : index_(index) {}
    // "compatibility" with packed type
    Id(Type, Arity, Index index)
        : Id(index) {}
    ~Id() {}

    Id(const Id& other) = default;

    bool operator==(const Id& other) const;
    bool operator!=(const Id& other) const;

    std::size_t hash() const;

    bool empty() const;

    Type type() const;
    Arity arity() const;
    Index index() const;

private:
    void reset();

    Index index_;
};

} // namespace stree


// ID hash function
template<> struct std::hash<stree::Id> {
    typedef stree::Id argument_type;
    typedef std::size_t result_type;
    result_type operator()(const argument_type& id) const {
        return id.hash();
    }
};

#endif
