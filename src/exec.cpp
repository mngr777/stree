#include <stree/exec.hpp>
#include <cassert>

namespace stree {

void Exec::init(Params* params, DataPtr data_ptr) {
    params_ = params;
    data_ptr_ = data_ptr;
    restart();
}

void Exec::run() {
    while (!is_finished())
        step();
}

void Exec::step() {
    if (is_finished())
        restart();

    Frame& current = stack_top();
    if (current.arguments.size() == current.argument_num) {
        switch (current.id.type()) {
            case TypeFunction: {
                Value value = eval(env_, current.id, *params_, data_ptr_);
                stack_pop();
                if (stack_empty()) {
                    // finished
                    is_finished_ = true;
                    result_ = value;
                } else {
                    // set parent argument to result
                    Frame& parent = stack_top();
                    parent.arguments.push_back(value);
                }
                break;
            }
            case TypeSelect: {
                unsigned branch = call_select_function(
                    env_, current.id, current.arguments, data_ptr_);
                // pop conditional, push selected branch
                stack_pop();
                if (branch < current.arguments.size()) {
                    // branch already evaluated as select function argument
                    Value value = current.arguments[branch];
                    if (stack_empty()) {
                        // finished
                        is_finished_ = true;
                        result_ = value;
                    } else {
                        // set parent argument to result
                        Frame& parent = stack_top();
                        parent.arguments.push_back(value);
                    }
                } else {
                    // push selected branch
                    Id branch_id = id::nth_argument(
                        env_.node_manager(), current.id, branch);
                    stack_push(branch_id);
                }
                // evaluating conditional doesn't count as a step
                step();
                break;
            }
            default:
                assert(false && "Invalid type");
        }

    } else {
        const Id& id = id::nth_argument(
            env_.node_manager(),
            current.id,
            current.arguments.size());
        if (id.arity() == 0) {
            Value value = eval(env_, id, *params_, data_ptr_);
            current.arguments.push_back(value);
            // evaluating constants and positionals doesn't count as a step
            if (id.type() == TypeConst || id.type() == TypePositional) {
                step();
            }
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
    Arity argument_num = id.arity();
    if (id.type() == TypeSelect) {
        const Symbol* symbol = env_.symbol(id);
        assert(symbol && "Select function symbol not found");
        argument_num = symbol->sf_arity();
    }
    stack_.emplace_back(id, argument_num);
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

std::ostream& ExecDebug::print_backtrace(
    std::ostream& os,
    PrintFlags flags) const
{
    if (exec_.stack_size() > 0) {
        for (unsigned n = 0; n < exec_.stack_size(); ++n) {
            os << "#" << n << " ";
            print_frame(os, exec_.stack_frame(n), flags);
            os << std::endl;
        }
    } else {
        os << "<empty>" << std::endl;
    }
    return os;
}

std::ostream& ExecDebug::print_frame(
    std::ostream& os,
    Exec::Frame& frame,
    PrintFlags flags) const
{
    // Print ID
    if (flags & PrintIds)
        os << frame.id << " ";

    // Print symbol name
    os << exec_.env_.symbol(frame.id)->name();

    // Print arguments
    os << " : ";
    for (const Value& value : frame.arguments) {
        os << value << " ";
    }

    // Print <empty> for missing arguments
    assert(frame.id.arity() >= frame.arguments.size());
    for (unsigned i = 0; i < frame.id.arity() - frame.arguments.size(); ++i) {
        os << "<empty> ";
    }

    return os;
}

}
