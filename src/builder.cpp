#include <stree/builder.hpp>
#include <cassert>
#include <stdexcept>

namespace stree {

Builder::Builder(Environment& env)
    : env_(env)
{
    reset();
}

void Builder::set(const std::string& name) {
    Id id = env_.make_id(name);
    set_id(id);
}

void Builder::set(const Symbol* symbol) {
    if (!symbol)
        throw std::invalid_argument("Symbol pointer is empty");
    Id id = env_.make_id(symbol);
    set_id(id);
}

void Builder::set(const Value& value) {
    Id id = env_.make_id(value);
    set_id(id);
}

void Builder::up(unsigned n) {
    if (stack_.size() > 1 && n > 0) {
        stack_.pop();
        up(n - 1);
    }
}

void Builder::down(unsigned n) {
    Id id = stack_.top().id;
    if (id.empty())
        throw std::invalid_argument("Current ID is empty");
    if (n + 1 > id.arity())
        throw std::invalid_argument("Argument number is too large");

    Id child_id = id::nth_argument(env_.node_manager(), id, n);
    stack_.emplace(child_id, n);
}

void Builder::reset() {
    while (!stack_.empty())
        stack_.pop();
    set_root(Id());
}

bool Builder::is_valid() const {
    return id::is_valid_subtree(env_.node_manager(), root_);
}

void Builder::set_id(const Id& id) {
    // destroy current sub
    if (!stack_.top().id.empty()) {
        id::destroy_subtree(env_.node_manager(), stack_.top().id);
    }

    if (stack_.size() == 1) {
        // Replace root
        stack_.pop();
        set_root(id);
    } else {
        // Set parent argument
        unsigned n = stack_.top().n; // store curren argument number
        up();
        Id parent_id = stack_.top().id;
        assert(!parent_id.empty());
        id::nth_argument(env_.node_manager(), parent_id, n) = id;
        down(n);
    }
}

void Builder::set_root(const Id& id) {
    assert(stack_.size() == 0);
    stack_.emplace(id, 0);
    root_ = id;
}

}
