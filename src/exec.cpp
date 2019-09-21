#include <stree/exec.hpp>
#include <cassert>

namespace stree {

void Exec::run(Params* const params, DataPtr data_ptr) {
    params_ = params;
    data_ptr_ = data_ptr_;
    restart();
}

void Exec::restart() {
    stack_clear();
    stack_push(root_);
    is_finished_ = false;
}

void Exec::step() {
    Frame& current = stack_top();
    if (current.arguments.size() == current.id.arity()) {
        Value value = eval(env_, current.id, *params_, data_ptr_);
        stack_pop();
        if (stack_empty()) {
            is_finished_ = true;
            result_ = value;
        } else {
            Frame& parent = stack_top();
            parent.arguments.push_back(value);
        }

    } else {
        const Id& id = id::nth_argument(
            env_.node_manager(),
            current.id,
            current.arguments.size());
        if (id.arity() == 0) {
            Value value = eval(env_, id, *params_, data_ptr_);
            current.arguments.push_back(value);
        } else {
            stack_push(id);
        }
    }
}

void Exec::stack_clear() {
    Stack stack;
    stack_.swap(stack);
}

void Exec::stack_push(const Id& id) {
    stack_.emplace(id);
}

void Exec::stack_pop() {
    stack_.pop();
}

Exec::Frame& Exec::stack_top() {
    return stack_.top();
}

bool Exec::stack_empty() const {
    return stack_.empty();
}

}
