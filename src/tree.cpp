#include <stree/tree.hpp>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <stree/environment.hpp>

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

const char* type_to_string(Type type) {
    switch (type) {
        case TypeConst:
            return "con";
        case TypePositional:
            return "pos";
        case TypeFunction:
            return "fun";
        case TypeSelect:
            return "sel";
    }
    assert(false);
}

Id::Id(Type type, Arity arity, Index index) {
    set_type(type);
    set_arity(arity);
    set_index(index);
}

Id& Id::operator=(const Id& other) {
    data_ = other.data_;
    return *this;
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


// Node manager

#define STREE_TMP_MEMBER_IMPL(_Type, _member)                   \
    template<>                                                  \
    Index NodeManager::alloc<_Type>() {                         \
        return _member.alloc();                                 \
    }                                                           \
    template<>                                                  \
    _Type& NodeManager::get<_Type>(Index index) {               \
        return _member.get(index);                              \
    }                                                           \
    template<>                                                  \
    const _Type& NodeManager::get<_Type>(Index index) const {   \
        return _member.get(index);                              \
    }                                                           \
    template<>                                                  \
    void NodeManager::free<_Type>(Index index) {                \
        _member.free(index);                                    \
    }

#define STREE_TMP_MEMBER_FUN_IMPL(_arity)                       \
    STREE_TMP_MEMBER_IMPL(FunctionNode<_arity>, fun ## _arity);

STREE_TMP_MEMBER_IMPL(Position, pos)
STREE_TMP_MEMBER_IMPL(Value, val)
STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MEMBER_FUN_IMPL)

#undef STREE_TMP_MEMBER_FUN_IMPL
#undef STREE_TMP_MEMBER_IMPL


namespace id {

#define STREE_TMP_MAKE_FUN_ARITY_CASE(_arity)       \
    else if (arity == _arity) {                     \
        index = nm.alloc<FunctionNode<_arity>>();   \
    }

Id make(NodeManager& nm, Type type, Arity arity) {
    Index index = Id::NoIndex;
    switch (type) {
        case TypeConst:
            index = nm.alloc<Value>();
            break;
        case TypePositional:
            index = nm.alloc<Position>();
            break;
        case TypeFunction:
            if (false) {}
            STREE_FOR_EACH_FUN_ARITY(STREE_TMP_MAKE_FUN_ARITY_CASE)
            else { assert(false && "Invalid arity"); }
            break;
        case TypeSelect:
            // TODO
            assert(false && "Not implemented");
            break;
    }
    return Id(type, arity, index);
}
#undef STREE_TMP_MAKE_FUN_ARITY_CASE


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


#define SMTREE_TMP_DESTROY_FUN_ARITY_CASE(_arity)   \
    else if(id.arity() == _arity) {                 \
        nm.free<FunctionNode<_arity>>(id.index());  \
    }

void destroy(NodeManager& nm, Id& id) {
    if (!id.empty()) {
        switch (id.type()) {
            case TypeConst:
                nm.free<Value>(id.index());
                break;
            case TypePositional:
                nm.free<Position>(id.index());
                break;
            case TypeFunction:
                if (false) {}
                STREE_FOR_EACH_FUN_ARITY(SMTREE_TMP_DESTROY_FUN_ARITY_CASE)
                else { assert(false && "Invalid arity"); }
                break;
            case TypeSelect:
                // TODO
                assert(false && "Not implemented");
                break;
        }
    }
    id.reset();
}
#undef SMTREE_TMP_DESTROY_FUN_ARITY_CASE


void destroy_subtree(NodeManager& nm, Id& root) {
    for (Arity n = 0; n < root.arity(); ++n)
        destroy_subtree(nm, nth_argument(nm, root, n));
    destroy(nm, root);
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
        return (nm.get<FunctionNode<_arity>>(id.index())).argument(n);  \
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
            else { assert(false && "Invalid arity"); }
            break;
        case TypeSelect:
            // TODO
            assert(false && "Not implemented");
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


/*
     (0)
     /  \
   (1)  (2)
   /    /  \
 (3)  (4)  (5)
*/
Id& nth_node(NodeManager& nm, Id& id, NodeNum n, IsTerminal is_terminal) {
    return const_cast<Id&>(
        nth_node(
            const_cast<const NodeManager&>(nm),
            const_cast<const Id&>(id),
            n,
            is_terminal));
}

const Id& nth_node(
    const NodeManager& nm,
    const Id& id,
    NodeNum n,
    IsTerminal is_terminal)
{
    _ConstNodeRefQueue queue;
    queue.emplace(id); // initialize queue with root
    while (!queue.empty()) {
        // next node from queue
        const Id& current = queue.front();
        queue.pop();
        assert(id::is_valid(nm, current) && "All nodes before N-th should be valid");
        bool terminality_match = (is_terminal == IsTerminalAny)
            || (is_terminal == IsTerminalYes && current.arity() == 0)
            || (is_terminal == IsTerminalNo && current.arity() > 0);
        if (n == 0 && terminality_match) {
            // found N-th node
            return current;
        } else if (
            is_terminal == IsTerminalAny
            && (n < queue.size() + 1 + current.arity()))
        {
            // one of current node children is N-th node
            // we can be sure only if we don't care if node is terminal
            assert(n > queue.size());
            return id::nth_argument(nm, current, n - 1 - queue.size());
        } else {
            // we don't know N-th node yet, add current node children to queue
            for (Arity i = 0; i < current.arity(); ++i)
                queue.emplace(id::nth_argument(nm, current, i));
            // decrement counter if current node terminality matches
            if (terminality_match)
                --n;
        }
    }
    throw std::range_error("Invalid node number");
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

Id copy(NodeManager& nm, const Id& id) {
    Id result = make(nm, id.type(), id.arity());
    switch (id.type()) {
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
            // TODO
            assert(false && "Not implemented");
            break;
    }
    return result;
}

Id copy_subtree(NodeManager& nm, const Id& root) {
    Id root_copy = copy(nm, root);
    for (Arity n = 0; n < root.arity(); ++n) {
        Id& arg = nth_argument(nm, root_copy, n);
        arg = copy_subtree(nm, nth_argument(nm, root, n));
    }
    return root_copy;
}

Value value(const NodeManager& nm, const Id& id) {
    assert(id.type() == TypeConst);
    return nm.get<Value>(id.index());
}

void set_value(NodeManager& nm, Id& id, Value value) {
    assert(id.type() == TypeConst);
    nm.get<Value>(id.index()) = std::move(value);
}

Position position(const NodeManager& nm, const Id& id) {
    assert(id.type() == TypePositional);
    return nm.get<Position>(id.index());
}

void set_position(NodeManager& nm, Id& id, Position position) {
    assert(id.type() == TypePositional);
    nm.get<Position>(id.index()) = position;
}


#define STREE_TMP_FID_FUN_ARITY_CASE(_arity)                    \
    else if (id.arity() == _arity) {                            \
        return nm.get<FunctionNode<_arity>>(id.index()).fid();  \
    }

FunctionIndex fid(const NodeManager& nm, const Id& id) {
    assert(id.type() == TypeFunction);
    if (false) {}
    STREE_FOR_EACH_FUN_ARITY(STREE_TMP_FID_FUN_ARITY_CASE)
    else { assert(false && "Invalid arity"); }
}
#undef STREE_TMP_FID_FUN_ARITY_CASE


#define STREE_TMP_SET_FID_FUN_ARITY_CASE(_arity)                \
    else if (id.arity() == _arity) {                            \
        nm.get<FunctionNode<_arity>>(id.index()).set_fid(fid);  \
    }

void set_fid(NodeManager& nm, Id& id, FunctionIndex fid) {
    assert(id.type() == TypeFunction);
    if (false) {}
    STREE_FOR_EACH_FUN_ARITY(STREE_TMP_SET_FID_FUN_ARITY_CASE)
    else { assert(false && "Invalid arity"); }
}
#undef STREE_TMP_SET_FID_FUN_ARITY_CASE

} // namespace id


// TreeBase::Description class

TreeDescription::TreeDescription() {
    set_null();
}

bool TreeDescription::is_empty() const {
    return (size == NoNodeNum)
        || (depth == NoNodeNum)
        || (term_num == NoNodeNum)
        || (nonterm_num == NoNodeNum);
}

void TreeDescription::set_null() {
    size = depth = term_num = nonterm_num = NoNodeNum;
}

void TreeDescription::set_zero() {
    size = depth = term_num = nonterm_num = 0;
}

TreeBase::TreeBase(Environment* env, TreeBase* parent)
    : env_(env), parent_(parent)
{
    assert(env_ && "Tree environment cannot be empty");
    reset_cache();
}

TreeBase::TreeBase(const TreeBase& other) {
    env_ = other.env_;
    parent_ = other.parent_;
    description_ = other.description_;
    width_ = other.width_;
}

TreeBase::TreeBase(TreeBase&& other)
    : TreeBase(other) {}

void TreeBase::set(const Symbol* symbol) {
    if (!symbol)
        throw std::invalid_argument("Empty symbol ptr");
    if (!root().empty() && symbol->arity() != root().arity())
        throw std::invalid_argument("Arity mismatch");
    // TODO: do not replace if type matches
    // Make replacement Id
    Id id = env_->make_id(symbol);
    if (!root().empty()) {
        // Copy (shallow) argument Ids
        for (Arity n = 0; n < root().arity(); ++n)
            id::nth_argument(env_->node_manager(), id, n) =
                id::nth_argument(env_->node_manager(), root(), n);
        // Destroy root (non-recursive)
        id::destroy(env_->node_manager(), root());
    }
    // Replace root
    root() = id;
}

void TreeBase::set(const std::string& name) {
    set(env_->symbol(name));
}

const Subtree TreeBase::sub(NodeNum n) const {
    return Subtree(
        env_,
        const_cast<TreeBase*>(this),
        const_cast<Id&>(id::nth_node(env_->node_manager(), root(), n)));
}

Subtree TreeBase::sub(NodeNum n) {
    return Subtree(env_, this, id::nth_node(env_->node_manager(), root(), n));
}

const Subtree TreeBase::argument(Arity n) const {
    check_argument_num(n);
    return Subtree(
        env_,
        const_cast<TreeBase*>(this),
        const_cast<Id&>(id::nth_argument(env_->node_manager(), root(), n)));
}

Subtree TreeBase::argument(Arity n) {
    check_argument_num(n);
    return Subtree(env_, this, id::nth_argument(env_->node_manager(), root(), n));
}

const Arity TreeBase::arity() const {
    return root().arity();
}

const TreeDescription& TreeBase::describe() const {
    if (description_.is_empty())
        update_description();
    return description_;
}

NodeNum TreeBase::width() const {
    if (width_ == NoNodeNum)
        update_width();
    return width_;
}

void TreeBase::reset_cache() {
    reset_description();
    reset_width();
    if (parent_)
        parent_->reset_cache();
}

void TreeBase::check_argument_num(Arity n) const {
    if (root().empty())
        throw std::invalid_argument("Root is empty");
    if (root().arity() < n + 1)
        throw std::range_error("Invalid argument number");
}

void TreeBase::update_description() const {
    description_.set_zero();
    id::for_each_node(
        env_->node_manager(),
        root(),
        [this](const Id& id, NodeNum n, NodeNum depth) {
            // size
            ++description_.size;
            // depth
            description_.depth = std::max(description_.depth, depth);
            // term_num and nonterm_num
            if (id.arity() == 0) {
                ++description_.term_num;
            } else {
                ++description_.nonterm_num;
            }
            return false;
        });
}

void TreeBase::update_width() const {
    width_ = id::subtree_width(env_->node_manager(), root());
}

void TreeBase::reset_description() {
    description_.set_null();
}

void TreeBase::reset_width() {
    width_ = NoNodeNum;
}


// Subtree class

void Subtree::swap(Subtree& other) {
    std::swap(root_, other.root_);
    reset_cache();
    other.reset_cache();
}

void Subtree::swap(Subtree&& other) {
    swap(other);
}

void Subtree::destroy() {
    id::destroy_subtree(env_->node_manager(), root_);
    reset_cache();
}

void Subtree::replace(Tree& tree) {
    destroy();
    Subtree subtree = tree.sub(0);
    swap(subtree);
}

Tree Subtree::copy() const {
    return Tree(env_, id::copy_subtree(env_->node_manager(), root_));
}


// Tree class

Tree::Tree(Environment* env, const Symbol* symbol)
    : TreeBase(env),
      root_(env->make_id(symbol)) {}

Tree::Tree(Tree&& other)
    : TreeBase(std::move(other))
{
    root_ = other.root_;
    other.root_.reset();
}

Tree::~Tree() {
    id::destroy_subtree(env_->node_manager(), root_);
}


} // namespace stree
