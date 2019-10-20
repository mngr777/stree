#ifndef STREE_EXEC_HPP_
#define STREE_EXEC_HPP_

#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stree/environment.hpp>
#include <stree/eval.hpp>
#include <stree/tree.hpp>

namespace stree {

class ExecCostLimitExceeded : public std::exception {
    const char* what() const noexcept;
};


class ExecDebug;

class Exec {
    friend ExecDebug;
public:
    using Flag = std::uint8_t;
    const static Flag NoFlags               = 0;
    const static Flag FlagRunLoop           = 1;
    const static Flag FlagStopOnFinished    = 2;
    const static Flag FlagStopIfCostNotZero = 4;
    const static Flag FlagStopOnFunctions   = 8;
    const static Flag FlagStopOnSelects     = 16;
    const static Flag DefaultFlags          = NoFlags; // FlagStopOnFinished;
    const static Flag AllFlags              = -1;

    Exec(const TreeBase& tree, Flag flags = DefaultFlags)
        : Exec(*tree.env(), tree.root(), flags) {}

    Exec(const Environment& env, const Id& root, Flag flags = DefaultFlags)
        : env_(env),
          root_(root),
          params_(nullptr),
          flags_(flags),
          cost_limit_(-1),
          cost_used_(0),
          data_ptr_(nullptr),
          is_finished_(false),
          result_(Value{}) {}

    void init(Params* params, DataPtr data_ptr = nullptr);
    void run();
    void step();
    void restart();

    void set_flag(Flag flag) {
        flags_ |= flag;
    }

    void unset_flag(Flag flag) {
        flags_ &= ~flag;
    }

    bool has_flag(Flag flag) {
        return flags_ & flag;
    }

    void set_cost_limit(Cost cost) {
        cost_limit_ = cost;
    }

    const Cost cost_limit() const {
        return cost_limit_;
    }

    const bool has_cost_limit() const {
        return cost_limit_ > 0;
    }

    const Cost cost_used() const {
        return cost_used_;
    }

    const bool is_finished() const {
        return is_finished_;
    }

    const Value& result() const {
        return result_;
    }

private:
    class Frame {
    public:
        Frame(const Id& id, Arity argument_num)
            : id(id), argument_num(argument_num) {}

        const Id id;
        const Arity argument_num;
        Arguments arguments;
    };

    using Stack = std::vector<Frame>;

    void eval_top();
    void push_argument();

    void stack_clear();
    void stack_push(const Id& id);
    void stack_return(Value value);
    void stack_pop();
    Frame& stack_top();
    bool stack_empty() const;

    // debug
    Frame& stack_frame(unsigned n);
    unsigned stack_size() const;

    const Environment& env_;
    const Id& root_;
    Params* params_;
    Flag flags_;
    Cost cost_limit_;
    Cost cost_used_;
    DataPtr data_ptr_;
    Stack stack_;

    bool is_finished_;
    Value result_;
};


class ExecDebug {
public:
    using PrintFlags = std::uint8_t;
    static const PrintFlags NoPrintFlags = 0;
    static const PrintFlags PrintIds     = 1;

    ExecDebug(Exec& exec) : exec_(exec) {}

    std::ostream& print_backtrace(
        std::ostream& os,
        PrintFlags flags = NoPrintFlags) const;

private:
    std::ostream& print_frame(
        std::ostream& os,
        Exec::Frame& frame,
        PrintFlags flags) const;

    Exec& exec_;
};

}

#endif
