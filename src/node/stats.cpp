#include <stree/node/stats.hpp>
#include <algorithm>
#include <stree/macros.hpp>
#include <stree/node.hpp>

namespace stree {

#define STREE_TMP_INIT_FUN_ARITY_STAT_ITEM(_arity)  \
    items_.emplace_back(TypeFunction, _arity);

#define STREE_TMP_INIT_SELECT_ARITY_STAT_ITEM(_arity)   \
    items_.emplace_back(TypeSelect, _arity);

NodeManagerStats::NodeManagerStats() {
    items_.emplace_back(TypeConst, 0);
    items_.emplace_back(TypePositional, 0);
    STREE_FOR_EACH_FUN_ARITY(STREE_TMP_INIT_FUN_ARITY_STAT_ITEM)
    STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_INIT_SELECT_ARITY_STAT_ITEM)
}

#undef STREE_TMP_INIT_FUN_ARITY_STAT_ITEM
#undef STREE_TMP_INIT_SELECT_ARITY_STAT_ITEM


#define STREE_TMP_GET_ITEM_STATS(_nm_member)                    \
    item.pool_size = nm._nm_member.nodes_.size();               \
    item.buffer_size = nm._nm_member.buffer_.size();

#define STREE_TMP_GET_FUN_ITEM_STATS(_arity)        \
    else if (item.arity == _arity) {                \
        STREE_TMP_GET_ITEM_STATS(fun ## _arity);    \
    }

#define STREE_TMP_GET_SELECT_ITEM_STATS(_arity)     \
    else if (item.arity == _arity) {                \
        STREE_TMP_GET_ITEM_STATS(select ## _arity); \
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
                else { assert(false && "Invalid function arity"); }
                break;
            case TypeSelect:
                if (false) {}
                STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_GET_SELECT_ITEM_STATS)
                else { assert(false && "Invalid select arity"); }
                break;
        }
    }
}

#undef STREE_TMP_GET_FUN_ITEM_STATS
#undef STREE_TMP_GET_SELECT_ITEM_STATS
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


bool operator==(
    const stree::NodeManagerStats::Item& item1,
    const stree::NodeManagerStats::Item& item2)
{
    return (item1.type == item2.type)
        && (item1.arity == item2.arity)
        && (item1.pool_size == item2.pool_size)
        && (item1.buffer_size == item2.buffer_size);
}

bool operator!=(
    const stree::NodeManagerStats::Item& item1,
    const stree::NodeManagerStats::Item& item2)
{
    return !(item1 == item2);
}

bool operator==(
    const stree::NodeManagerStats& stats1,
    const stree::NodeManagerStats& stats2)
{
    auto items1 = stats1.items();
    auto items2 = stats2.items();
    assert(items1.size() == items2.size());
    stree::NodeManagerStats::ItemList::size_type i = 0;
    for (; i < items1.size(); ++i)
        if (items1[i] != items2[i])
            return false;
    return true;
}

bool operator!=(const stree::NodeManagerStats& s1, const stree::NodeManagerStats& s2) {
    return !(s1 == s2);
}

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
