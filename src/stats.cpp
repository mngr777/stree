#include <stree/stats.hpp>
#include <algorithm>
#include <stdexcept>
#include <stree/macros.hpp>

namespace stree {

#define STREE_TMP_INIT_FUN_ARITY_STAT_ITEM(_arity)  \
    items_.emplace_back(TypeFunction, _arity);

NodeManagerStats::NodeManagerStats() {
    items_.emplace_back(TypeConst, 0);
    items_.emplace_back(TypePositional, 0);
    STREE_FOR_EACH_FUN_ARITY(STREE_TMP_INIT_FUN_ARITY_STAT_ITEM)
}
#undef STREE_TMP_INIT_FUN_ARITY_STAT_ITEM


#define STREE_TMP_GET_ITEM_STATS(_nm_member)                    \
    item.pool_size = nm._nm_member.nodes_.size();               \
    item.buffer_size = nm._nm_member.buffer_.size();

#define STREE_TMP_GET_FUN_ITEM_STATS(_arity)        \
    else if (item.arity == _arity) {                \
        STREE_TMP_GET_ITEM_STATS(fun ## _arity);    \
    }

void NodeManagerStats::update(const NodeManager& nm) {
    for (auto &item : items_) {
        switch (item.type) {
            case TypeConst:
                STREE_TMP_GET_ITEM_STATS(val)
                break;
            case TypePositional:
                STREE_TMP_GET_ITEM_STATS(pos)
                break;
            case TypeFunction:
                if (false) {}
                STREE_FOR_EACH_FUN_ARITY(STREE_TMP_GET_FUN_ITEM_STATS)
                else { assert(false && "Invalid arity"); }
                break;
            case TypeSelect:
                // TODO
                assert(false && "Not implemented");
                break;
        }
    }
}
#undef STREE_TMP_GET_FUN_ITEM_STATS
#undef STREE_TMP_GET_ITEM_STATS


const NodeManagerStats::Item NodeManagerStats::item(Type type, Arity arity) const {
    auto it = std::find_if(
        items_.begin(), items_.end(),
        [type, arity](const Item& item) {
            return item.type == type && item.arity == arity;
        });
    if (it == items_.end())
        throw std::invalid_argument("Stats item not found");
    return *it;
}

} // namespace stree


std::ostream& operator<<(std::ostream& os, const stree::NodeManagerStats::Item& item) {
    // Type
    os << "[" << stree::type_to_string(item.type);
    // Arity
    if (item.type == stree::TypeFunction || item.type == stree::TypeSelect)
        os << " " << static_cast<unsigned>(item.arity);
    os << "] ";
    // Pool size
    assert(item.pool_size >= item.buffer_size);
    os << item.pool_size << " nodes, "
       << (item.pool_size - item.buffer_size) << " used, "
       << item.buffer_size << " in buffer"
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const stree::NodeManagerStats& stats) {
    for (auto &item : stats.items())
        os << item;
    return os;
}
