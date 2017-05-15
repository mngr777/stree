#ifndef STREE_TREE_HPP_
#define STREE_TREE_HPP_

#include <cstdint>
#include <limits>
#include <mutex>
#include <ostream>
#include <queue>
#include <vector>

// Arity width in bits
#ifndef STREE_ARITY_WIDTH
# define STREE_ARITY_WIDTH (3)
#endif

// Value type
#ifndef STREE_VALUE_TYPE
# define STREE_VALUE_TYPE float;
#endif

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
using FunctionIndex = std::uint8_t;
using Position = std::uint8_t;
using Value = STREE_VALUE_TYPE;

// Function index "empty" value
const FunctionIndex FunctionNoIndex = std::numeric_limits<FunctionIndex>::max();


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

    Id() : Id(TypeConst, 0, NoIndex) {}

    Id(Type type, Arity arity, Index index);

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

    Id& argument(Arity n) {
        return arguments_[n];
    }

    const Id& argument(Arity n) const {
        return arguments_[n];
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

    void free(Index index) {
        LockGuard lg(mtx_);
        buffer_.push(index);
    }

private:
    std::vector<T> nodes_;
    std::queue<Index> buffer_;
    std::mutex mtx_;
};


// Node manager

#define STREE_TMP_MEMBER_DECL(_Type, _member)   \
    private:                                    \
    NodePool<_Type> _member;                    \

#define STREE_TMP_MEMBER_FUN_DECL(_arity)                       \
    STREE_TMP_MEMBER_DECL(FunctionNode<_arity>, fun ## _arity);

class NodeManager {
public:
    Id make(Type type, Arity arity = 0);

    void destroy(Id id);

    template<typename T>
    Index alloc_node();

    template<typename T>
    T& get_node(Index index);

    template<typename T>
    void free_node(Index index);

    STREE_TMP_MEMBER_DECL(Position, pos)
    STREE_TMP_MEMBER_DECL(Value, val)
    STREE_TMP_MEMBER_FUN_DECL(0)
    STREE_TMP_MEMBER_FUN_DECL(1)
    STREE_TMP_MEMBER_FUN_DECL(2)
};

#undef STREE_TMP_MEMBER_FUN_DECL
#undef STREE_TMP_MEMBER_DECL

} // namespace stree

#endif
