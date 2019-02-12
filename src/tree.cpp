#include <stree/tree.hpp>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <stree/environment.hpp>


namespace stree {

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

bool TreeBase::is_valid() const {
    return id::is_valid_subtree(env_->node_manager(), root());
}

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

void TreeBase::set(Value value) {
    Symbol symbol("", TypeConst);
    symbol.set_value(std::move(value));
    set(&symbol);
}

const Subtree TreeBase::sub(NodeNum n, IsTerminal is_terminal) const {
    return Subtree(
        env_,
        const_cast<TreeBase*>(this),
        const_cast<Id&>(id::nth_node(env_->node_manager(), root(), n, is_terminal)));
}

Subtree TreeBase::sub(NodeNum n, IsTerminal is_terminal) {
    return Subtree(
        env_, this, id::nth_node(env_->node_manager(), root(), n, is_terminal));
}

const Subtree TreeBase::term(NodeNum n) const {
    return sub(n, IsTerminalYes);
}

Subtree TreeBase::term(NodeNum n) {
    return sub(n, IsTerminalYes);
}

const Subtree TreeBase::nonterm(NodeNum n) const {
    return sub(n, IsTerminalNo);
}

Subtree TreeBase::nonterm(NodeNum n) {
    return sub(n, IsTerminalNo);
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

Tree::Tree(Environment* env, const Value& value)
    : TreeBase(env),
      root_(env->make_id(value)) {}

Tree::Tree(const Tree& other)
    : TreeBase(other)
{
    copy(other);
}

Tree::Tree(Tree&& other)
    : TreeBase(std::move(other))
{
    copy(std::move(other));
}

Tree::~Tree() {
    id::destroy_subtree(env_->node_manager(), root_);
}

Tree& Tree::operator=(const Tree& other) {
    TreeBase::operator=(other);
    copy(other);
    return *this;
}

Tree& Tree::operator=(Tree&& other) {
    TreeBase::operator=(std::move(other));
    copy(std::move(other));
    return *this;
}

void Tree::copy(const Tree& other) {
    id::destroy_subtree(env_->node_manager(), root_);
    root_ = id::copy_subtree(env_->node_manager(), other.root_);
}

void Tree::copy(Tree&& other) {
    id::destroy_subtree(env_->node_manager(), root_);
    root_ = other.root_;
    other.root_.reset();
}


} // namespace stree
