#ifndef STREE_TREE_HPP_
#define STREE_TREE_HPP_

#include <cstdint>
#include <limits>
#include <mutex>
#include <ostream>
#include <queue>
#include <vector>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>
#include <stree/macros.hpp>

// List of function arities
#ifndef STREE_FUN_ARITY_TUPLE
# define STREE_FUN_ARITY_TUPLE (0, 1, 2)
#endif

// Arity width in bits
#ifndef STREE_ARITY_WIDTH
# define STREE_ARITY_WIDTH (3)
#endif

// Value type
#ifndef STREE_VALUE_TYPE
# define STREE_VALUE_TYPE float;
#endif


// Helper macros

// Apply MACRO to each value in ARITY_TUPLE:
// MACRO(arity)
#define STREE_FOR_EACH_FUN_ARITY(MACRO)                                 \
    BOOST_PP_LIST_FOR_EACH(                                             \
        STREE_FOR_EACH_WRAPPER, MACRO, BOOST_PP_TUPLE_TO_LIST(STREE_FUN_ARITY_TUPLE))

// Apply MACRO to each index and value in ARITY_TUPLE:
// MACRO(index, arity)
//
// May be used to map arity values stored in Id class to actual arity,
// reducing required number of bits: O(log(MaxArity)) to O(log(ArityTupleSize))
#define STREE_FOR_EACH_FUN_ARITY_I(MACRO)                               \
    BOOST_PP_LIST_FOR_EACH_I(                                           \
        STREE_FOR_EACH_WRAPPER_I, MACRO, BOOST_PP_TUPLE_TO_LIST(STREE_FUN_ARITY_TUPLE))


// Output
namespace stree {
class Id;
}
std::ostream& operator<<(std::ostream& os, const stree::Id id);


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


// Tree node ID
// Contains node type, arity and index in node pool
class Id {
private:
    static constexpr Index DataMask = std::numeric_limits<Index>::max();
    static constexpr Index TypeMask = (DataMask >> (ArityWidth + IndexWidth)) << (ArityWidth + IndexWidth);
    static constexpr Index IndexMask = (DataMask << (TypeWidth + ArityWidth)) >> (TypeWidth + ArityWidth);
    static constexpr Index ArityMask = DataMask & ~TypeMask & ~IndexMask;

public:
    static constexpr Index NoIndex = IndexMask;

    // Id() : Id(TypeConst, 0, NoIndex) {}
    Id() : data_(NoIndex) {}

    Id(Type type, Arity arity, Index index);

    // void reset() {
    //     set_index(NoIndex);
    // }

    bool empty() const {
        return index() == NoIndex;
    }

    Type type() const;
    void set_type(Type type);

    Arity arity() const;
    void set_arity(Arity arity);

    Index index() const;
    void set_index(Index index);

private:
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

    Id argument(Arity n) const {
        return arguments_[n];
    }

    void set_argument(Arity n, Id argument_id) {
        arguments_[n] = argument_id;
    }

private:
    FunctionIndex fid_;
    Id arguments_[A];
};


// Pool of nodes of specific type
template<typename T>
class NodePool {
private:
    using LockGuard = std::lock_guard<std::mutex>;
public:
    Index alloc() {
        LockGuard lg(mtx_);
        if (buffer_.empty()) {
            nodes_.emplace_back(T());
            return nodes_.size() - 1;
        } else {
            Index index = buffer_.front();
            buffer_.pop();
            return index;
        }
    }

    T& get(Index index) {
        LockGuard lg(mtx_);
        return nodes_[index];
    }

    const T& get(Index index) const {
        LockGuard lg(mtx_);
        return nodes_[index];
    }

    void free(Index index) {
        LockGuard lg(mtx_);
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
public:
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


namespace id {

Id make(NodeManager& nm, Type type, Arity arity = 0);

void destroy(NodeManager& nm, Id id);
void destroy_subtree(NodeManager& nm, Id root);

Id nth_argument(const NodeManager& nm, Id id, Arity n);
void set_nth_argument(NodeManager& nm, Id id, Arity n, Id argument_id);

Id copy(NodeManager& nm, Id id);
Id copy_subtree(NodeManager& nm, Id root);

// Const
Value value(const NodeManager& nm, Id id);
void set_value(NodeManager& nm, Id id, Value value);

// Positional
Position position(const NodeManager& nm, Id id);
void set_position(NodeManager& nm, Id id, Position position);

// Function
FunctionIndex fid(const NodeManager& nm, Id id);
void set_fid(NodeManager& nm, Id id, FunctionIndex fid);

// Select
// TODO
// SelectFunctionIndex sfid(NodeManager& nm, Id id);
// void set_sfid(NodeManager& nm, Id id, SelectFunctionIndex sfid);

} // namespace id


class Environment;

class Tree {
public:
    Tree(Environment* env) : env_(env) {}

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
    Id root_;
    Environment* env_;
};

} // namespace stree

#endif
