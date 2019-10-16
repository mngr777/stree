#ifndef STREE_TREE_HPP_
#define STREE_TREE_HPP_

#include <string>
#include <stree/environment/symbol.hpp>
#include <stree/macros.hpp>
#include <stree/types.hpp>
#include <stree/search.hpp>
#include <stree/node.hpp>

namespace stree {

class TreeDescription {
public:
    TreeDescription();

    bool is_empty() const;
    void set_null();
    void set_zero();

    NodeNum size;
    NodeNum depth;
    NodeNum term_num;
    NodeNum nonterm_num;
};

class Environment;
class Tree;
class Subtree;

class TreeBase {
public:
    TreeBase(Environment* env, TreeBase* parent = nullptr);
    virtual ~TreeBase() {}

    virtual Id& root() = 0;
    virtual const Id& root() const = 0;

    bool is_valid() const;

    void swap(TreeBase& other);
    void swap(TreeBase&& other);

    // Replace root with ID for other symbol with same arity
    void set(const SymbolPtr& symbol);
    void set(const std::string& name);
    void set(Value value);

    const Subtree sub(NodeNum n) const;
    Subtree sub(NodeNum n);

    const Subtree sub(NodeNum n, const NodeFilter& filter) const;
    Subtree sub(NodeNum n, const NodeFilter& filter);

    // n-th terminal
    const Subtree term(NodeNum n) const;
    Subtree term(NodeNum n);

    // n-th non-terminal
    const Subtree nonterm(NodeNum n) const;
    Subtree nonterm(NodeNum n);

    const Subtree argument(Arity n) const;
    Subtree argument(Arity n);

    const Arity arity() const;

    const TreeDescription& describe() const;

    NodeNum width() const;

    void reset_cache();

    const Environment* env() const {
        return env_;
    }

    Environment* env() {
        return env_;
    }

protected:
    Environment* env_;
    TreeBase* parent_;

    void check_argument_num(Arity n) const;
    void update_description() const;
    void update_width() const;
    void reset_description();
    void reset_width();

private:
    mutable TreeDescription description_;
    mutable NodeNum width_;
};

class Subtree : public TreeBase {
public:
    Subtree(
        Environment* env,
        TreeBase* parent,
        Id& root)
        : TreeBase(env, parent),
          root_(root) {}

    void destroy();

    // NOTE: `tree' root will be replaced with empty node
    void replace(Tree& tree);
    void replace(Tree&& tree);

    // Create tree from subtree copy
    Tree copy() const;

    virtual const Id& root() const {
        return root_;
    }

    virtual Id& root() {
        return root_;
    }

private:
    Id& root_;
};


class Tree : public TreeBase {
public:
    Tree(Environment* env)
        : Tree(env, Id()) {}

    Tree(Environment* env, Id root)
        : TreeBase(env, nullptr),
          root_(root) {}

    Tree(Environment* env, const SymbolPtr& symbol);
    Tree(Environment* env, const Value& value);

    Tree(const Tree& other);
    Tree(Tree&& other);

    Tree& operator=(const Tree& other);
    Tree& operator=(Tree&& other);

    virtual ~Tree();

    virtual const Id& root() const {
        return root_;
    }

    virtual Id& root() {
        return root_;
    }

private:
    Id root_;

    void copy(const Tree& other);
    void copy(Tree&& other);
};

} // namespace stree

#endif
