#include <stree/exec.hpp>
#include <cassert>

namespace stree {

void Exec::init(Params* params, DataPtr data_ptr) {
    params_ = params;
    data_ptr_ = data_ptr_;
    restart();
}

void Exec::run() {
    while (!is_finished())
        step();
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
            step();
        }
    }
}

void Exec::restart() {
    stack_clear();
    stack_push(root_);
    is_finished_ = false;
}

void Exec::stack_clear() {
    stack_.clear();
}

void Exec::stack_push(const Id& id) {
    stack_.emplace_back(id);
}

void Exec::stack_pop() {
    stack_.pop_back();
}

Exec::Frame& Exec::stack_top() {
    return stack_.back();
}

bool Exec::stack_empty() const {
    return stack_.empty();
}

Exec::Frame& Exec::stack_frame(unsigned n) {
    return stack_[n];
}

unsigned Exec::stack_size() const {
    return stack_.size();
}

std::ostream& ExecDebug::print_backtrace(std::ostream& os) const {
    if (exec_.stack_size() > 0) {
        for (unsigned n = 0; n < exec_.stack_size(); ++n)
            print_frame(os, exec_.stack_frame(n));
    } else {
        os << "<empty>";
    }
    return os;
}

std::ostream& ExecDebug::print_frame(std::ostream& os, Exec::Frame& frame) const {
    os << frame.id << " "
       << exec_.env_.symbol(frame.id)->name() << " ";
    for (const Value& value : frame.arguments) {
        os << value << " ";
    }
    assert(frame.id.arity() >= frame.arguments.size());
    for (unsigned i = 0; i < frame.id.arity() - frame.arguments.size(); ++i) {
        os << "<empty>";
    }
    return os;
}

}
