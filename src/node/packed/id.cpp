#include <stree/node/packed/id.hpp>
#include <stree/node/packed/manager.hpp>
#include <stree/search.hpp>

std::ostream& operator<<(std::ostream& os, const stree::Id& id) {
    os << '(';
    if (!id.empty()) {
        os << "type: " << type_to_string(id.type())
           << ", arity: " << static_cast<unsigned>(id.arity())
           << ", index: " << id.index();
    } else {
        os << "empty";
    }
    return os << ')';
}


namespace {

template<typename D, typename V>
V get(D data, D mask, std::uint8_t offset) {
    return (data & mask) >> offset;
}

template<typename D, typename V>
void set(D& data, D mask, std::uint8_t offset, V value) {
    data = (data & ~mask)
        | ((static_cast<D>(value) << offset)
           & mask);
}

} // namespace


namespace stree {

Id::Id(Type type, Arity arity, Index index) {
    set_type(type);
    set_arity(arity);
    set_index(index);
}

bool Id::operator==(const Id& other) const {
    return data_ == other.data_;
}

bool Id::operator!=(const Id& other) const {
    return !(*this == other);
}

std::size_t Id::hash() const {
    return data_;
}

Type Id::type() const {
    return type_id_to_type(
        get<Index, TypeId>(data_, TypeMask, IndexWidth + ArityWidth));
}

void Id::set_type(Type type) {
    set(data_, TypeMask, IndexWidth + ArityWidth, type_to_type_id(type));
}

Arity Id::arity() const {
    return get<Index, Arity>(data_, ArityMask, IndexWidth);
}

void Id::set_arity(Arity arity) {
    set(data_, ArityMask, IndexWidth, arity);
}

Index Id::index() const {
    return get<Index, Index>(data_, IndexMask, 0);
}

void Id::set_index(Index index) {
    set(data_, IndexMask, 0, index);
}


namespace id {

#define STREE_TMP_MAKE_FUN_ARITY_CASE(_arity)                   \
    else if (arity == _arity) {                                 \
        index = nm.alloc<FunctionNode<_arity>, TypeFunction>(); \
    }

#define STREE_TMP_MAKE_SELECT_ARITY_CASE(_arity)            \
    else if (arity == _arity) {                             \
        index = nm.alloc<SelectNode<_arity>, TypeSelect>(); \
    }

Id make(NodeManager& nm, Type type, Arity arity) {
    Index index = Id::NoIndex;
    switch (type) {
    case TypeConst:
        index = nm.alloc<Value, TypeConst>();
        break;
    case TypePositional:
        index = nm.alloc<Position, TypePositional>();
        break;
    case TypeFunction:
        if (false) {}
        STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MAKE_FUN_ARITY_CASE)
        else { assert(false && "Invalid function arity"); }
        break;
    case TypeSelect:
        if (false) {}
        STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_MAKE_SELECT_ARITY_CASE)
        else { assert(false && "Invalid select arity"); }
        break;
    }
    assert(index != Id::NoIndex);
    return Id(type, arity, index);
}

#undef STREE_TMP_MAKE_FUN_ARITY_CASE
#undef STREE_TMP_MAKE_SELECT_ARITY_CASE


// TODO: use map (see subtree_width)
NodeNum subtree_size(const NodeManager& nm, const Id& id) {
    NodeNum size = 0;
    if (!id.empty()) {
        ++size;
        for (NodeNum n = 0; n < id.arity(); ++n)
            size += subtree_size(nm, id::nth_argument(nm, id, n));
    }
    return size;
}

NodeNum subtree_width(const NodeManager& nm, const Id& id) {
    _NodeWidthMap width_map;
    return _subtree_width(nm, id, width_map);
}

NodeNum _subtree_width(
    const NodeManager& nm,
    const Id& id,
    _NodeWidthMap& width_map)
{
    // Look up in map
    auto it = width_map.find(id);
    if (it != width_map.end())
        return (*it).second;

    // Calc. width
    NodeNum width = 0;
    for (Arity n = 0; n < id.arity(); ++n)
        width += _subtree_width(nm, nth_argument(nm, id, n), width_map);
    width = std::max(static_cast<NodeNum>(1), width);

    // Store in map
    width_map[id] = width;

    return width;
}


#define STREE_TMP_DESTROY_FUN_ARITY_CASE(_arity)                    \
    else if(id.arity() == _arity) {                                 \
        nm.free<FunctionNode<_arity>, TypeFunction>(id.index());    \
    }

#define STREE_TMP_DESTROY_SELECT_ARITY_CASE(_arity)             \
    else if (id.arity() == _arity) {                            \
        nm.free<SelectNode<_arity>, TypeSelect>(id.index());    \
    }

void destroy(NodeManager& nm, Id& id) {
    if (!id.empty()) {
        switch (id.type()) {
        case TypeConst:
            nm.free<Value, TypeConst>(id.index());
            break;
        case TypePositional:
            nm.free<Position, TypePositional>(id.index());
            break;
        case TypeFunction:
            if (false) {}
            STREE_FOR_EACH_FUN_ARITY(STREE_TMP_DESTROY_FUN_ARITY_CASE)
            else { assert(false && "Invalid function arity"); }
            break;
        case TypeSelect:
            if (false) {}
            STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_DESTROY_SELECT_ARITY_CASE)
            else { assert(false && "Invalid select arity"); }
            break;
        }
        id.reset();
    }
}

#undef STREE_TMP_DESTROY_FUN_ARITY_CASE
#undef STREE_TMP_DESTROY_SELECT_ARITY_CASE


void destroy_subtree(NodeManager& nm, Id& root) {
    if (!root.empty()) {
        for (Arity n = 0; n < root.arity(); ++n)
            destroy_subtree(nm, nth_argument(nm, root, n));
        destroy(nm, root);
    }
}

bool is_valid(const NodeManager& nm, const Id& id) {
    return !id.empty();
}

bool is_valid_subtree(const NodeManager& nm, const Id& root) {
    if (root.empty())
        return false;
    for (Arity n = 0; n < root.arity(); ++n)
        if (!is_valid_subtree(nm, nth_argument(nm, root, n)))
            return false;
    return true;
}


#define STREE_TMP_ARGUMENT_FUN_ARITY_CASE(_arity)                       \
    else if (id.arity() == _arity) {                                    \
        return nm.get<FunctionNode<_arity>, TypeFunction>(id.index()).argument(n); \
    }

#define STREE_TMP_ARGUMENT_SELECT_ARITY_CASE(_arity)                    \
    else if (id.arity() == _arity) {                                    \
        return nm.get<SelectNode<_arity>, TypeSelect>(id.index()).argument(n); \
    }

const Id& nth_argument(const NodeManager& nm, const Id& id, Arity n) {
    switch (id.type()) {
    case TypeConst:
    case TypePositional:
        assert(false && "Nodes of this type cannot have arguments");
        break;
    case TypeFunction:
        if (false) {}
        STREE_FOR_EACH_FUN_ARITY(STREE_TMP_ARGUMENT_FUN_ARITY_CASE)
        else { assert(false && "Invalid function arity"); }
        break;
    case TypeSelect:
        if (false) {}
        STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_ARGUMENT_SELECT_ARITY_CASE)
        else { assert(false && "Invalid select arity"); }
        break;
    }
    assert(false && "Unknown type");
}

Id& nth_argument(NodeManager& nm, Id& id, Arity n) {
    return const_cast<Id&>(
        nth_argument(
            const_cast<const NodeManager&>(nm),
            const_cast<const Id&>(id),
            n));
}

#undef STREE_TMP_ARGUMENT_FUN_ARITY_CASE
#undef STREE_TMP_ARGUMENT_SELECT_ARITY_CASE


Id copy(NodeManager& nm, const Id id) {
    Id result;
    if (!id.empty()) {
        result = make(nm, id.type(), id.arity());
        switch (result.type()) {
        case TypeConst:
            set_value(nm, result, value(nm, id));
            break;
        case TypePositional:
            set_position(nm, result, position(nm, id));
            break;
        case TypeFunction:
            set_fid(nm, result, fid(nm, id));
            break;
        case TypeSelect:
            set_sfid(nm, result, sfid(nm, id));
            break;
        }
    }
    return result;
}

Id copy_subtree(NodeManager& nm, const Id root) {
    Id root_copy = copy(nm, root);
    if (!root.empty()) {
        for (Arity n = 0; n < root.arity(); ++n) {
            Id arg_copy = copy_subtree(nm, nth_argument(nm, root, n));
            nth_argument(nm, root_copy, n) = arg_copy;
        }
    }
    return root_copy;
}

Value value(const NodeManager& nm, const Id& id) {
    assert(id.type() == TypeConst);
    return nm.get<Value, TypeConst>(id.index());
}

void set_value(NodeManager& nm, Id& id, Value value) {
    assert(id.type() == TypeConst);
    nm.get<Value, TypeConst>(id.index()) = std::move(value);
}

Position position(const NodeManager& nm, const Id& id) {
    assert(id.type() == TypePositional);
    return nm.get<Position, TypePositional>(id.index());
}

void set_position(NodeManager& nm, Id& id, Position position) {
    assert(id.type() == TypePositional);
    nm.get<Position, TypePositional>(id.index()) = position;
}


#define STREE_TMP_FID_FUN_ARITY_CASE(_arity)                            \
    else if (id.arity() == _arity) {                                    \
        return nm.get<FunctionNode<_arity>, TypeFunction>(id.index()).fid(); \
    }

FunctionIndex fid(const NodeManager& nm, const Id& id) {
    assert(id.type() == TypeFunction);
    if (false) {}
    STREE_FOR_EACH_FUN_ARITY(STREE_TMP_FID_FUN_ARITY_CASE)
    else { assert(false && "Invalid function arity"); }
}
#undef STREE_TMP_FID_FUN_ARITY_CASE


#define STREE_TMP_SET_FID_FUN_ARITY_CASE(_arity)                        \
    else if (id.arity() == _arity) {                                    \
        nm.get<FunctionNode<_arity>, TypeFunction>(id.index()).set_fid(fid); \
    }

void set_fid(NodeManager& nm, Id& id, FunctionIndex fid) {
    assert(id.type() == TypeFunction);
    if (false) {}
    STREE_FOR_EACH_FUN_ARITY(STREE_TMP_SET_FID_FUN_ARITY_CASE)
    else { assert(false && "Invalid function arity"); }
}
#undef STREE_TMP_SET_FID_FUN_ARITY_CASE


#define STREE_TMP_SFID_SELECT_ARITY_CASE(_arity)                        \
    else if (id.arity() == _arity) {                                    \
        return nm.get<SelectNode<_arity>, TypeSelect>(id.index()).sfid(); \
    }

SelectFunctionIndex sfid(const NodeManager& nm, const Id& id) {
    assert(id.type() == TypeSelect);
    if (false) {}
    STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_SFID_SELECT_ARITY_CASE)
    else { assert(false && "Invalid select arity"); }
}

#undef STREE_TMP_SFID_SELECT_ARITY_CASE


#define STREE_TMP_SET_SFID_SELECT_ARITY_CASE(_arity)                    \
    else if (id.arity() == _arity) {                                    \
        nm.get<SelectNode<_arity>, TypeSelect>(id.index()).set_sfid(sfid); \
    }

void set_sfid(NodeManager& nm, Id& id, SelectFunctionIndex sfid) {
    assert(id.type() == TypeSelect);
    if (false) {}
    STREE_FOR_EACH_SELECT_ARITY(STREE_TMP_SET_SFID_SELECT_ARITY_CASE)
    else { assert(false && "Invalid select arity"); }
}

#undef STREE_TMP_SET_SFID_SELECT_ARITY_CASE

// TODO: move to `algorithm'

/*
    (0)
    /  \
  (1)  (2)
  /    /  \
(3)  (4)  (5)
*/

const Id& nth_node(const NodeManager& nm, const Id& id, NodeNum n) {
    return nth_node(nm, id, n, NodeFilter());
}

Id& nth_node(NodeManager& nm, Id& id, NodeNum n) {
    return nth_node(nm, id, n, NodeFilter());
}

const Id& nth_node(const NodeManager& nm, const Id& id, NodeNum n, const NodeFilter& filter) {
    _ConstNodeRefQueue queue;
    queue.emplace(id); // initialize queue with root
    while (!queue.empty()) {
        // next node from queue
        const Id& current = queue.front();
        queue.pop();
        assert(id::is_valid(nm, current) && "All nodes before N-th should be valid");
        bool match = filter.match(current);
        if (n == 0 && match) {
            // found N-th node
            return current;
        } else if (filter.empty() && (n < queue.size() + 1 + current.arity())) {
            // one of current node children is N-th node
            // we can be sure only if we don't filter nodes
            assert(n > queue.size());
            return id::nth_argument(nm, current, n - 1 - queue.size());
        } else {
            // we don't know N-th node yet, add current node children to queue
            for (Arity i = 0; i < current.arity(); ++i)
                queue.emplace(id::nth_argument(nm, current, i));
            // decrement counter if current node matches filter
            if (match) --n;
        }
    }
    throw std::range_error("Invalid node number");
}

Id& nth_node(NodeManager& nm, Id& id, NodeNum n, const NodeFilter& filter) {
    return const_cast<Id&>(
        nth_node(
            const_cast<const NodeManager&>(nm),
            const_cast<const Id&>(id),
            n, filter));
}

void for_each_node(
    const NodeManager& nm,
    const Id& id,
    std::function<bool(const Id&, NodeNum, NodeNum)> callback)
{
    assert(id::is_valid_subtree(nm, id) && "Subtree must be valid");
    // Node number
    NodeNum current_num = 0;
    // Init queue
    _ConstNodeRefDepthPairQueue queue;
    queue.emplace(_ConstNodeRefDepthPair(id, 0));
    while (!queue.empty()) {
        // Get current node
        const Id& current = queue.front().first;
        NodeNum current_depth = queue.front().second;
        queue.pop();
        // Callback. Return in result is true
        if (callback(current, current_num, current_depth))
            return;
        // Inc. node number
        ++current_num;
        // Add all children to queue
        for (Arity i = 0; i < current.arity(); ++i)
            queue.emplace(
                _ConstNodeRefDepthPair(
                    id::nth_argument(nm, current, i),
                    current_depth + 1));
    }
}

} // namespace id

} // namespace stree
