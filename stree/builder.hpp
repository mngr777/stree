#ifndef STREE_BUILDER_HPP_
#define STREE_BUILDER_HPP_

#include <stack>
#include <stree/environment.hpp>
#include <stree/tree.hpp>

namespace stree {

class Builder {
public:
    Builder(Environment& env);

    void set(const std::string& name);
    void set(const Symbol* symbol);
    void set(const Value& value);
    void up(unsigned n = 1);
    void down(unsigned n);

    bool is_valid() const;

    Id root() {
        return root_;
    }

private:
    struct StackItem {
        StackItem(const Id& id, unsigned n)
            : id(id),
              n(n) {}
        Id id;
        unsigned n;
    };
    using Stack = std::stack<StackItem>;
    Id root_;

    void set_id(const Id& id);
    void set_root(const Id& id);

    Environment& env_;
    Stack stack_;
};

}

#endif
