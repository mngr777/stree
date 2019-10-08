#ifndef STREE_NODE_STATS_HPP_
#define STREE_NODE_STATS_HPP_

#include <cstdlib>
#include <ostream>
#include <vector>
#include <stree/types.hpp>

namespace stree {

class NodeManager;

class NodeManagerStats {
public:
    struct Item {
        Item(Type type, Arity arity)
            : type(type),
              arity(arity),
              pool_size(0),
              buffer_size(0) {}
        Type type;
        Arity arity;
        std::size_t pool_size;
        std::size_t buffer_size;
    };
    typedef std::vector<Item> ItemList;

    NodeManagerStats();

    NodeManagerStats(const NodeManager& nm)
        : NodeManagerStats()
    {
        update(nm);
    }

    void update(const NodeManager& nm);

    const Item item(Type type, Arity arity = 0) const;

    const ItemList& items() const {
        return items_;
    }

private:
    ItemList items_;
};

} // namespace stree

bool operator==(
    const stree::NodeManagerStats::Item& item1,
    const stree::NodeManagerStats::Item& item2);

bool operator!=(
    const stree::NodeManagerStats::Item& iitem1,
    const stree::NodeManagerStats::Item& item2);

bool operator==(
    const stree::NodeManagerStats& stats1,
    const stree::NodeManagerStats& stats2);

bool operator!=(
    const stree::NodeManagerStats& stats1,
    const stree::NodeManagerStats& stats2);

std::ostream& operator<<(std::ostream& os, const stree::NodeManagerStats::Item& item);

std::ostream& operator<<(std::ostream& os, const stree::NodeManagerStats& stats);

#endif
