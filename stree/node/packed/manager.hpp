#ifndef STREE_NODE_PACKED_NODE_MANAGER_HPP_
#define STREE_NODE_PACKED_NODE_MANAGER_HPP_

#include <cassert>
#include <mutex> // TODO: remove mutex
#include <queue>
#include <vector>
#include <stree/types.hpp>
#include <stree/macros.hpp>
#include <stree/node/packed/id.hpp>

namespace stree {

// Function tree node
template<Arity A>
class FunctionNode {
public:
    FunctionNode() : FunctionNode(FunctionNoIndex) {}

    FunctionNode(FunctionIndex fid) : fid_(fid) {}

    FunctionIndex fid() const {
        return fid_;
    }

    void set_fid(FunctionIndex fid) {
        fid_ = fid;
    }

    const Id& argument(Arity n) const {
        assert(0 <= n && n < A);
        return arguments_[n];
    }

    Id& argument(Arity n) {
        assert(0 <= n && n < A);
        return arguments_[n];
    }

    void set_argument(Arity n, Id arg) {
        assert(0 <=n && n < A);
        arguments_[n] = arg;
    }

private:
    FunctionIndex fid_;
    Id arguments_[A];
};


template<Arity A>
class SelectNode {
public:
    SelectNode() : SelectNode(SelectFunctionNoIndex) {}

    SelectNode(SelectFunctionIndex sfid) : sfid_(sfid) {}

    SelectFunctionIndex sfid() const {
        return sfid_;
    }

    void set_sfid(FunctionIndex sfid) {
        sfid_ = sfid;
    }

    const Id& argument(Arity n) const {
        assert(0 <= n && n < A);
        return arguments_[n];
    }

    Id& argument(Arity n) {
        assert(0 <= n && n < A);
        return arguments_[n];
    }

    void set_argument(Arity n, Id arg) {
        assert(0 <=n && n < A);
        arguments_[n] = arg;
    }

private:
    SelectFunctionIndex sfid_;
    Id arguments_[A];
};


class NodeManagerStats;

// Pool of nodes of specific type
template<typename T>
class NodePool {
private:
    friend NodeManagerStats;
    using LockGuard = std::lock_guard<std::mutex>;
public:
    NodePool() {}
    NodePool(const NodePool& other) = delete;
    NodePool& operator=(const NodePool& other) = delete;

    Id::Index alloc() {
        LockGuard lg(mtx_);
        Id::Index index = Id::NoIndex;
        if (buffer_.empty()) {
            index = nodes_.size();
            nodes_.emplace_back(T());
        } else {
            index = buffer_.front();
            buffer_.pop();
        }
        assert(Id::NoIndex);
        return index;
    }

    T& get(Id::Index index) {
        assert(index != Id::NoIndex);
        assert(0 <= index && index < nodes_.size());
        LockGuard lg(mtx_);
        return nodes_[index];
    }

    const T& get(Id::Index index) const {
        assert(index != Id::NoIndex);
        assert(0 <= index && index < nodes_.size());
        LockGuard lg(mtx_);
        return nodes_[index];
    }

    void free(Id::Index index) {
        assert(index != Id::NoIndex);
        assert(0 <= index && index < nodes_.size());
        LockGuard lg(mtx_);
        buffer_.push(index);
    }

private:
    std::vector<T> nodes_;
    std::queue<Id::Index> buffer_;
    mutable std::mutex mtx_;
};


// Node manager

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
