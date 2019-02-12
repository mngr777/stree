#ifndef STREE_NODE_PACKED_ID_HPP_
#define STREE_NODE_PACKED_ID_HPP_

#include <functional>
#include <ostream>
#include <queue>
#include <unordered_map>
#include <stree/macros.hpp>
#include <stree/search.hpp>
#include <stree/types.hpp>

// Output
namespace stree {
class Id;
}
std::ostream& operator<<(std::ostream& os, const stree::Id& id);


namespace stree {

// Bit field widths
const std::uint8_t TypeWidth = 2;
const std::uint8_t ArityWidth = STREE_ARITY_WIDTH;
static_assert(TypeWidth + ArityWidth < sizeof(Index) * 8);
static_assert(ArityWidth < sizeof(FunctionIndex) * 8);
constexpr std::uint8_t IndexWidth = (sizeof(Index) * 8 - TypeWidth - ArityWidth);


class NodeManager;
class Id;

namespace id {

using _NodeRefQueue = std::queue<std::reference_wrapper<Id>>;
using _ConstNodeRefQueue = std::queue<std::reference_wrapper<const Id>>;

using _ConstNodeRefDepthPair = std::pair<const Id&, NodeNum>;
using _ConstNodeRefDepthPairQueue = std::queue<_ConstNodeRefDepthPair>;

Id make(NodeManager& nm, Type type, Arity arity = 0);

NodeNum subtree_size(const NodeManager& nm, const Id& id);

using _NodeWidthMap = std::unordered_map<Id, NodeNum>;

NodeNum subtree_width(const NodeManager& nm, const Id& id);
NodeNum _subtree_width(
    const NodeManager& nm,
    const Id& id,
    _NodeWidthMap& width_map);

void destroy(NodeManager& nm, Id& id);
void destroy_subtree(NodeManager& nm, Id& root);

bool is_valid(const NodeManager& nm, const Id& id);
bool is_valid_subtree(const NodeManager& nm, const Id& root);

const Id& nth_argument(const NodeManager& nm, const Id& id, Arity n);
Id& nth_argument(NodeManager& nm, Id& id, Arity n);

Id& nth_node(
    NodeManager& nm,
    Id& id,
    NodeNum n,
    IsTerminal is_terminal = IsTerminalAny);
const Id& nth_node(
    const NodeManager& nm,
    const Id& id,
    NodeNum n,
    IsTerminal is_terminal = IsTerminalAny);

void for_each_node(
    const NodeManager& nm,
    const Id& id,
    std::function<bool(const Id&, NodeNum, NodeNum)> callback);

// NOTE: cannot pass by reference for copying
Id copy(NodeManager& nm, const Id id);
Id copy_subtree(NodeManager& nm, const Id root);

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


class Tree;

class Id {
    friend void id::destroy(NodeManager& nm, Id& id);
    friend Tree;

private:
    static constexpr Index DataMask = std::numeric_limits<Index>::max();
    static constexpr Index TypeMask = (DataMask >> (ArityWidth + IndexWidth)) << (ArityWidth + IndexWidth);
    static constexpr Index IndexMask = (DataMask << (TypeWidth + ArityWidth)) >> (TypeWidth + ArityWidth);
    static constexpr Index ArityMask = DataMask & ~TypeMask & ~IndexMask;

public:
    static constexpr Index NoIndex = IndexMask;

    Id() : data_(NoIndex) {}
    Id(Type type, Arity arity, Index index);

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


namespace std {
// ID hash function
template<> struct hash<stree::Id> {
    typedef stree::Id argument_type;
    typedef std::size_t result_type;
    result_type operator()(const argument_type& id) const {
        return id.hash();
    }
};

} // namespace std

#endif
