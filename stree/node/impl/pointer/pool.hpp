#ifndef STREE_NODE_IMPL_POINTER_NODE_MANAGER_HPP_
#define STREE_NODE_IMPL_POINTER_NODE_MANAGER_HPP_

#include <cassert>
#include <queue>
#include <stree/types.hpp>
#include <stree/macros.hpp>
#include <stree/node/impl/pointer/id.hpp>
#include <stree/node/impl/pointer/node.hpp>

namespace stree {

class NodeManagerStats;

// Pool of nodes of specific type
template<typename T>
class NodePool {
    friend NodeManagerStats;

public:
    NodePool() {}
    NodePool(const NodePool& other) = delete;

    ~NodePool() {
        for (Id::Index index : nodes_)
            delete static_cast<T*>(index);
    }

    NodePool& operator=(const NodePool& other) = delete;

    Id::Index alloc() {
        Id::Index index = Id::NoIndex;
        if (buffer_.empty()) {
            index = new T();
            nodes_.push_back(index);
        } else {
            index = buffer_.front();
            buffer_.pop();
        }
        assert(index != Id::NoIndex);
        return index;
    }

    T& get(Id::Index index) {
        assert(index != Id::NoIndex);
        return static_cast<T&>(*index);
    }

    const T& get(Id::Index index) const {
        assert(index != Id::NoIndex);
        return static_cast<T&>(*index);
    }

    void free(Id::Index index) {
        assert(index != Id::NoIndex);
        buffer_.push(index);
    }

private:
    std::vector<Id::Index> nodes_;
    std::queue<Id::Index> buffer_;
};

} // namespace stree

#endif
