#ifndef STREE_EXEC_HPP_
#define STREE_EXEC_HPP_

#include <ostream>
#include <string>
#include <vector>
#include <stree/environment.hpp>
#include <stree/eval.hpp>
#include <stree/tree.hpp>

namespace stree {

class ExecDebug;

class Exec {
    friend ExecDebug;
public:
    Exec(const TreeBase& tree)
        : Exec(*tree.env(), tree.root()) {}

    Exec(const Environment& env, const Id& root)
        : env_(env),
          root_(root),
          params_(nullptr),
          data_ptr_(nullptr),
          is_finished_(false),
          result_(Value{}) {}

    void init(Params* params, DataPtr data_ptr = nullptr);
    void run();
    void step();
    void restart();

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

        const Id& id;
        Arity argument_num;
        Arguments arguments;
    };

    using Stack = std::vector<Frame>;

    void stack_clear();
    void stack_push(const Id& id);
    void stack_pop();
    Frame& stack_top();
    bool stack_empty() const;

    // debug
    Frame& stack_frame(unsigned n);
    unsigned stack_size() const;

    const Environment& env_;
    const Id& root_;
    Params* params_;
    DataPtr data_ptr_;
    Stack stack_;

    bool is_finished_;
    Value result_;
};


class ExecDebug {
public:
    using PrintFlags = char;
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
