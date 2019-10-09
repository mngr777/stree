#ifndef STREE_NODE_IMPL_PACKED_ID_HPP_
#define STREE_NODE_IMPL_PACKED_ID_HPP_

#include <functional>
#include <stree/macros.hpp>
#include <stree/node/functions.hpp>
#include <stree/types.hpp>

namespace stree {

class Tree;
class NodeManager;

class Id {
    friend void id::destroy(NodeManager& nm, Id& id);
    friend Tree;

public:
    using Index = std::uint32_t;

private:
// Bit field widths
    static const std::uint8_t TypeWidth = 2;
    static constexpr std::uint8_t ArityWidth = STREE_ARITY_WIDTH;
    static_assert(TypeWidth + ArityWidth < sizeof(Index) * 8);
    static_assert(ArityWidth < sizeof(FunctionIndex) * 8);
    static constexpr std::uint8_t IndexWidth = (sizeof(Index) * 8 - TypeWidth - ArityWidth);

    static constexpr Index DataMask = std::numeric_limits<Index>::max();
    static constexpr Index TypeMask = (DataMask >> (ArityWidth + IndexWidth)) << (ArityWidth + IndexWidth);
    static constexpr Index IndexMask = (DataMask << (TypeWidth + ArityWidth)) >> (TypeWidth + ArityWidth);
    static constexpr Index ArityMask = DataMask & ~TypeMask & ~IndexMask;

public:
    static constexpr Index NoIndex = IndexMask;

    Id() : data_(NoIndex) {}
    Id(Type type, Arity arity, Index index);
    Id(const Id& other) = default;
    ~Id() {}

    bool operator==(const Id& other) const;
    bool operator!=(const Id& other) const;

    std::size_t hash() const;

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
