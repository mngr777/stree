#ifndef STREE_NODE_HPP_
#define STREE_NODE_HPP_

#include <functional>
#include <ostream>
#include <queue>
#include <unordered_map>
#include <stree/macros.hpp>
#include <stree/search.hpp>
#include <stree/types.hpp>
#include <stree/node/packed.hpp>

std::ostream& operator<<(std::ostream& os, const stree::Id& id);

namespace std {

// ID hash function
template<> struct hash<stree::Id> {
    typedef stree::Id argument_type;
    typedef std::size_t result_type;
    result_type operator()(const argument_type& id) const {
        return id.hash();
    }
};

}


namespace stree { namespace id {

using _NodeRefQueue = std::queue<std::reference_wrapper<Id>>;
using _ConstNodeRefQueue = std::queue<std::reference_wrapper<const Id>>;

using _ConstNodeRefDepthPair = std::pair<const Id&, NodeNum>;
using _ConstNodeRefDepthPairQueue = std::queue<_ConstNodeRefDepthPair>;


NodeNum subtree_size(const NodeManager& nm, const Id& id);

using _NodeWidthMap = std::unordered_map<Id, NodeNum>;

NodeNum subtree_width(const NodeManager& nm, const Id& id);
NodeNum _subtree_width(
    const NodeManager& nm,
    const Id& id,
    _NodeWidthMap& width_map);

void destroy_subtree(NodeManager& nm, Id& root);

bool is_valid(const NodeManager& nm, const Id& id);
bool is_valid_subtree(const NodeManager& nm, const Id& root);

const Id& nth_argument(const NodeManager& nm, const Id& id, Arity n);
Id& nth_argument(NodeManager& nm, Id& id, Arity n);

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
SelectFunctionIndex sfid(const NodeManager& nm, const Id& id);
void set_sfid(NodeManager& nm, Id& id, SelectFunctionIndex sfid);


// TODO: move traversal outside
const Id& nth_node(const NodeManager& nm, const Id& id, NodeNum n);
Id& nth_node(NodeManager& nm, Id& id, NodeNum n);

const Id& nth_node(const NodeManager& nm, const Id& id, NodeNum n, const NodeFilter& filter);
Id& nth_node(NodeManager& nm, Id& id, NodeNum n, const NodeFilter& filter);

void for_each_node(
    const NodeManager& nm,
    const Id& id,
    std::function<bool(const Id&, NodeNum, NodeNum)> callback);

}}

#endif
