#include <stree/exec.hpp>
#include <cassert>

namespace stree {

const char* ExecCostLimitExceeded::what() const noexcept {
    return "Cost limit exceeded";
}


void Exec::init(Params* params, DataPtr data_ptr) {
    params_ = params;
    data_ptr_ = data_ptr;
    restart();
}

void Exec::run() {
    bool loop = has_flag(FlagRunLoop);
    while (loop || !is_finished())
        step();
}

void Exec::step() {
    if (is_finished())
        restart();

    Frame& current = stack_top();
    if (current.arguments.size() == current.argument_num) {
        // Arguments evaluated, evaluate top
        eval_top();
    } else {
        // Push next argument
        push_argument();
    }
}

void Exec::eval_top() {
    Frame& current = stack_top();

    // Check cost
    Cost cost = 0;
    SymbolPtr symbol;
    if (current.id.type() != TypeConst) {
        symbol = env_.symbols().by_id(current.id);
        cost = symbol->cost();
    }
    if (has_cost_limit() && (cost_used_ + cost) > cost_limit_) {
        throw ExecCostLimitExceeded();
    }

    switch (current.id.type()) {
        case TypeConst:
        case TypePositional: {
            Value value = eval(env_, current.id, *params_, data_ptr_);
            stack_return(value);
            break;
        }
        case TypeFunction: {
            Value value = call_function(
                env_, current.id, current.arguments, data_ptr_);
            stack_return(value);
            break;
        }
        case TypeSelect: {
            unsigned branch = call_select_function(
                env_, current.id, current.arguments, data_ptr_);
            if (branch < current.arguments.size()) {
                // branch already evaluated as select function argument
                Value value = current.arguments[branch];
                stack_return(value);
            } else {
                // push selected branch
                Id branch_id = id::nth_argument(
                    env_.node_manager(), current.id, branch);
                stack_pop();
                stack_push(branch_id);
            }
            break;
        }
    }

    // Update cost counter
    cost_used_ += cost;

    // Check if need to stop
    bool stop = false;
    stop |= (is_finished() && has_flag(FlagStopOnFinished));
    stop |= (cost > 0 && has_flag(FlagStopIfCostNotZero));
    if (symbol) {
        stop |= (symbol->type() == TypeFunction && has_flag(FlagStopOnFunctions));
        stop |= (symbol->type() == TypeSelect && has_flag(FlagStopOnSelects));
    }
    if (!stop) step();
}

void Exec::push_argument() {
    Frame& current = stack_top();
    const Id& id = id::nth_argument(
        env_.node_manager(),
        current.id,
        current.arguments.size());
    stack_push(id);
    step();
}

void Exec::restart() {
    stack_clear();
    stack_push(root_);
    is_finished_ = false;
    cost_used_ = 0.0;
}

void Exec::stack_clear() {
    stack_.clear();
}

void Exec::stack_push(const Id& id) {
    Arity argument_num = get_argument_num(env_, id);
    stack_.emplace_back(id, argument_num);
}

void Exec::stack_return(Value value) {
    stack_pop();
    if (stack_empty()) {
        // finished
        is_finished_ = true;
        result_ = value;
    } else {
        // set next argument to result
        Frame& current = stack_top();
        assert(
            current.arguments.size() < current.argument_num
            && "Too many arguments");
        current.arguments.push_back(value);
    }
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
    os << exec_.env_.symbols().by_id(frame.id)->name();

    // Print arguments
    os << " : ";
    for (const Value& value : frame.arguments) {
        os << value << " ";
    }

    // Print <empty> for missing arguments
    assert(frame.argument_num >= frame.arguments.size());
    for (unsigned i = 0; i < frame.argument_num - frame.arguments.size(); ++i) {
        os << "<empty> ";
    }

    return os;
}

}
