#ifndef STREE_STATS_HPP_
#define STREE_STATS_HPP_

#include <cstdlib>
#include <ostream>
#include <stree/tree.hpp>

namespace stree {

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

std::ostream& operator<<(std::ostream& os, const stree::NodeManagerStats::Item& item);

std::ostream& operator<<(std::ostream& os, const stree::NodeManagerStats& stats);

#endif