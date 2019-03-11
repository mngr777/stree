#ifndef STREE_ALGORITHM_
#define STREE_ALGORITHM_

#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>
#include <stree/node.hpp>
#include <stree/types.hpp>
#include <stree/compare.hpp>
#include <stree/environment.hpp>

namespace stree {

using NodeRef = std::reference_wrapper<Id>;

class CommonRegion {
public:
    struct Item {
        Item(NodeNum n, NodeRef node1, NodeRef node2)
            : n(n), node1(node1), node2(node2) {}
        NodeNum n;
        NodeRef node1;
        NodeRef node2;
    };
    typedef std::vector<Item> ItemList;

    void add(NodeNum n, NodeRef node1, NodeRef node2);

    const Item& get(NodeNum n) const;
    Item& get(NodeNum n);

    Item& operator[](unsigned index);
    const Item& operator[](unsigned index) const;

    ItemList::iterator begin();
    ItemList::const_iterator begin() const;

    ItemList::iterator end();
    ItemList::const_iterator end() const;


private:
    ItemList list_;
    std::unordered_map<NodeNum, Item> map_;
};

CommonRegion common_region(
    Environment* env,
    Id& root1, Id& root2,
    const NodeCompare& compare);

} // namespace stree

#endif
