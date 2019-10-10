#ifndef STREE_NODE_IMPL_PACKED_POOL_HPP_
#define STREE_NODE_IMPL_PACKED_POOL_HPP_

#include <cassert>
#include <queue>
#include <vector>
#include <stree/node/impl/packed/id.hpp>
#include <stree/node/impl/packed/node.hpp>

namespace stree {

class NodeManagerStats;

// Pool of nodes of specific type
template<typename T>
class NodePool {
    friend NodeManagerStats;

public:
    NodePool() {}
    NodePool(const NodePool& other) = delete;
    NodePool& operator=(const NodePool& other) = delete;

    Id::Index alloc() {
        Id::Index index = Id::NoIndex;
        if (buffer_.empty()) {
            index = nodes_.size();
            nodes_.emplace_back(T());
        } else {
            index = buffer_.front();
            buffer_.pop();
        }
        assert(index != Id::NoIndex);
        return index;
    }

    T& get(Id::Index index) {
        assert(index != Id::NoIndex);
        assert(0 <= index && index < nodes_.size());
        return nodes_[index];
    }

    const T& get(Id::Index index) const {
        assert(index != Id::NoIndex);
        assert(0 <= index && index < nodes_.size());
        return nodes_[index];
    }

    void free(Id::Index index) {
        assert(index != Id::NoIndex);
        assert(0 <= index && index < nodes_.size());
        buffer_.push(index);
    }

private:
    std::vector<T> nodes_;
    std::queue<Id::Index> buffer_;
};

} // namespace stree

#endif
