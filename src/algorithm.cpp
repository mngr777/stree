#include <stree/algorithm.hpp>
#include <algorithm>
#include <queue>

namespace stree {

namespace {

using NodeRefPair = std::pair<NodeRef, NodeRef>;
using NodeRefPairQueue = std::queue<NodeRefPair>;

} // anonymous namespace


void CommonRegion::add(NodeNum n, NodeRef node1, NodeRef node2) {
    list_.emplace_back(n, node1, node2);
    map_.emplace(n, Item(n, node1, node2));
    assert(node1.get().arity() == node2.get().arity());
    if (node1.get().arity() > 0) {
        terminal_num_list_.push_back(n);
    } else{
        nonterminal_num_list_.push_back(n);
    }
}

const CommonRegion::Item& CommonRegion::get(NodeNum n) const {
    return map_.at(n);
}

CommonRegion::Item& CommonRegion::get(NodeNum n) {
    return map_.at(n);
}

const CommonRegion::Item& CommonRegion::operator[](unsigned index) const {
    return list_.at(index);
}

CommonRegion::Item& CommonRegion::operator[](unsigned index) {
    return list_.at(index);
}

NodeNum CommonRegion::size() const {
    return list_.size();
}

NodeNum CommonRegion::terminal_num() const {
    return terminal_num_list_.size();
}

CommonRegion::Item& CommonRegion::nth_terminal(NodeNum n) {
    return map_.at(terminal_num_list_.at(n));
}

const CommonRegion::Item& CommonRegion::nth_terminal(NodeNum n) const {
    return map_.at(terminal_num_list_.at(n));
}

NodeNum CommonRegion::nonterminal_num() const {
    return nonterminal_num_list_.size();
}

CommonRegion::Item& CommonRegion::nth_nonterminal(NodeNum n) {
    return map_.at(nonterminal_num_list_.at(n));
}

const CommonRegion::Item& CommonRegion::nth_nonterminal(NodeNum n) const {
    return map_.at(nonterminal_num_list_.at(n));
}

CommonRegion::ItemList::iterator CommonRegion::begin() {
    return list_.begin();
}

CommonRegion::ItemList::const_iterator CommonRegion::begin() const {
    return list_.cbegin();
}

CommonRegion::ItemList::iterator CommonRegion::end() {
    return list_.end();
}

CommonRegion::ItemList::const_iterator CommonRegion::end() const {
    return list_.cend();
}


CommonRegion common_region(
    Environment* env,
    Id& root1, Id& root2,
    const NodeCompare& compare)
{
    // NodeNumList result;
    CommonRegion result;
    NodeNum current_node_num = 0;

    NodeRefPairQueue queue;
    if (compare.equal(root1, root2)) {
        // Initialize queue
        queue.emplace(root1, root2);
        // Add nodes to result, increment node counter
        // result.push_back(current_node_num++);
        result.add(current_node_num++, root1, root2);
    }

    // Collect matching nodes
    while (!queue.empty()) {
        // Get equal nodes from queues
        Id& current1 = queue.front().first;
        Id& current2 = queue.front().second;
        queue.pop();

        // NOTE: assuming arity match is required
        assert(current1.arity() == current2.arity());

        for (Arity i = 0; i < current1.arity(); ++i) {
            Id& child1 = id::nth_argument(env->node_manager(), current1, i);
            Id& child2 = id::nth_argument(env->node_manager(), current2, i);
            if (compare.equal(child1, child2)) {
                // Add equal nodes to queue
                queue.emplace(child1, child2);
                // Add node num to result
                // result.push_back(current_node_num);
                result.add(current_node_num, child1, child2);
            }
            // Increment node counter
            ++current_node_num;
        }
    }

    return result;
}

} // namespace stree
