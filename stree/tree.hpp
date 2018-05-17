#ifndef STREE_TREE_HPP_
#define STREE_TREE_HPP_

#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <mutex>
#include <ostream>
#include <queue>
#include <vector>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>
#include <stree/macros.hpp>

// Output
namespace stree {
class Id;
}
std::ostream& operator<<(std::ostream& os, const stree::Id& id);


namespace stree {

// Basic types
using TypeId = std::uint8_t;
using Arity  = std::uint8_t;
using Index  = std::uint32_t;
using FunctionIndex       = std::uint8_t;
using SelectFunctionIndex = std::uint8_t;
using Position = std::uint8_t;
using Value    = STREE_VALUE_TYPE;

// "empty" values
const FunctionIndex FunctionNoIndex =
    std::numeric_limits<FunctionIndex>::max();
const SelectFunctionIndex SelectFunctionNoIndex =
    std::numeric_limits<SelectFunctionIndex>::max();


// Type enum and conversions

enum Type : TypeId {
    TypeConst      = 0,
    TypePositional = 1,
    TypeFunction   = 2,
    TypeSelect     = 3
};

inline Type type_id_to_type(TypeId id) {
    return static_cast<Type>(id);
}

inline TypeId type_to_type_id(Type type) {
    return static_cast<TypeId>(type);
}

const char* type_to_string(Type type);


// Bit field widths
const std::uint8_t TypeWidth = 2;
const std::uint8_t ArityWidth = STREE_ARITY_WIDTH;
static_assert(TypeWidth + ArityWidth < sizeof(Index) * 8);
static_assert(ArityWidth < sizeof(FunctionIndex) * 8);
constexpr std::uint8_t IndexWidth = (sizeof(Index) * 8 - TypeWidth - ArityWidth);


class NodeManager;

// Tree node number type
using NodeNum = unsigned;
// "empty" tree node number
const NodeNum NoNodeNum = std::numeric_limits<NodeNum>::max();

namespace id {

using _NodeRefQueue = std::queue<std::reference_wrapper<Id>>;
using _ConstNodeRefQueue = std::queue<std::reference_wrapper<const Id>>;

Id make(NodeManager& nm, Type type, Arity arity = 0);

NodeNum subtree_size(const NodeManager& nm, const Id& id);

void destroy(NodeManager& nm, Id& id);
void destroy_subtree(NodeManager& nm, Id& root);

bool is_valid(const NodeManager& nm, const Id& id);
bool is_valid_subtree(const NodeManager& nm, const Id& root);

const Id& nth_argument(const NodeManager& nm, const Id& id, Arity n);
Id& nth_argument(NodeManager& nm, Id& id, Arity n);

Id& nth_node(NodeManager& nm, Id& id, NodeNum n);
const Id& nth_node(const NodeManager& nm, const Id& id, NodeNum n);

Id copy(NodeManager& nm, const Id& id);
Id copy_subtree(NodeManager& nm, const Id& root);

// Const
Value value(const NodeManager& nm, const Id& id);
void set_value(NodeManager& nm, Id& id, Value value);

// Positional
Position position(const NodeManager& nm, const Id& id);
void set_position(NodeManager& nm, Id& id, Position position);

// Function
FunctionIndex fid(const NodeManager& nm, const Id& id);
void set_fid(NodeManager& nm, Id& id, FunctionIndex fid);

// Select
// TODO
// SelectFunctionIndex sfid(NodeManager& nm, Id id);
// void set_sfid(NodeManager& nm, Id id, SelectFunctionIndex sfid);

} // namespace id

// Tree node ID
// Contains node type, arity and index in node pool
class Id {
    friend void id::destroy(NodeManager& nm, Id& id);

private:
    static constexpr Index DataMask = std::numeric_limits<Index>::max();
    static constexpr Index TypeMask = (DataMask >> (ArityWidth + IndexWidth)) << (ArityWidth + IndexWidth);
    static constexpr Index IndexMask = (DataMask << (TypeWidth + ArityWidth)) >> (TypeWidth + ArityWidth);
    static constexpr Index ArityMask = DataMask & ~TypeMask & ~IndexMask;

public:
    static constexpr Index NoIndex = IndexMask;

    Id() : data_(NoIndex) {}
    Id(Type type, Arity arity, Index index);
    Id(const Id& other) : data_(other.data_) {}
    Id& operator=(const Id& other) {
        data_ = other.data_;
        return *this;
    }

    bool empty() const {
        return index() == NoIndex;
    }

    Type type() const;
    Arity arity() const;
    Index index() const;

private:
    void reset() {
        data_ = NoIndex;
    }

    void set_type(Type type);
    void set_arity(Arity arity);
    void set_index(Index index);

    Index data_;
};


// Function tree node
template<Arity A>
class FunctionNode {
public:
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

    void unset_argument(Arity n) {
        assert(0 <=n && n < A);
        // ??
    }

private:
    FunctionIndex fid_;
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

    Index alloc() {
        LockGuard lg(mtx_);
        Index index = 0;
        if (buffer_.empty()) {
            index = nodes_.size();
            nodes_.emplace_back(T());
        } else {
            index = buffer_.front();
            buffer_.pop();
        }
        return index;
    }

    T& get(Index index) {
        LockGuard lg(mtx_);
        assert(0 <= index && index < nodes_.size());
        return nodes_[index];
    }

    const T& get(Index index) const {
        LockGuard lg(mtx_);
        assert(0 <= index && index < nodes_.size());
        return nodes_[index];
    }

    void free(Index index) {
        LockGuard lg(mtx_);
        assert(0 <= index && index < nodes_.size());
        buffer_.push(index);
    }

private:
    std::vector<T> nodes_;
    std::queue<Index> buffer_;
    mutable std::mutex mtx_;
};


// Node manager

#define STREE_TMP_MEMBER_DECL(_Type, _member)   \
    private:                                    \
    NodePool<_Type> _member;                    \

#define STREE_TMP_MEMBER_FUN_DECL(_arity)                       \
    STREE_TMP_MEMBER_DECL(FunctionNode<_arity>, fun ## _arity);

class NodeManager {
    friend NodeManagerStats;
public:
    NodeManager() {}
    NodeManager(const NodeManager& other) = delete;
    NodeManager& operator=(const NodeManager& other) = delete;

    template<typename T>
    Index alloc();

    template<typename T>
    T& get(Index index);

    template<typename T>
    const T& get(Index index) const;

    template<typename T>
    void free(Index index);

    STREE_TMP_MEMBER_DECL(Position, pos)
    STREE_TMP_MEMBER_DECL(Value, val)
    STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MEMBER_FUN_DECL)
};

#undef STREE_TMP_MEMBER_FUN_DECL
#undef STREE_TMP_MEMBER_DECL


class Environment;
class Tree;

class Subtree {
public:
    Subtree(
        Environment* env,
        Id& root)
        : env_(env),
          root_(root) {}

    void swap(Subtree& other);

    void destroy();

    // NOTE: `tree' root will be replaced with empty node
    void replace(Tree& tree);

    // Create tree from subtree copy
    Tree copy();

    const Id& root() const {
        return root_;
    }

    Id& root() {
        return root_;
    }

    const Environment* env() const {
        return env_;
    }

    Environment* env() {
        return env_;
    }

private:
    Environment* env_;
    Id& root_;
};


class Tree {
public:
    Tree(Environment* env)
        : Tree(env, Id()) {}

    Tree(Environment* env, Id root)
        : env_(env),
          root_(root),
          size_cache_(NoNodeNum)
    {
        assert(env && "Environment pointer cannot be empty");
    }

    ~Tree();

    NodeNum size() const;

    const Subtree subtree(NodeNum n) const;
    Subtree subtree(NodeNum n);

    const Id& root() const {
        return root_;
    }

    Id& root() {
        return root_;
    }

    const Environment* env() const {
        return env_;
    }

    Environment* env() {
        return env_;
    }

private:
    Environment* env_;
    Id root_;
    mutable NodeNum size_cache_;
};

} // namespace stree

#endif
