#ifndef STREE_EXEC_HPP_
#define STREE_EXEC_HPP_

#include <stack>
#include <stree/environment.hpp>
#include <stree/eval.hpp>
#include <stree/tree.hpp>

namespace stree {

class Exec {
public:
    class Frame {
    public:
        Frame(const Id& id): id(id) {}

        const Id& id;
        Arguments arguments;
    };

    Exec(const Environment& env, const Id& root)
        : env_(env),
          root_(root),
          params_(nullptr),
          data_ptr_(nullptr),
          is_finished_(false),
          result_(Value{}) {}

    void run(Params* params, DataPtr data_ptr = nullptr);
    void restart();
    void step();

    const bool is_finished() const {
        return is_finished_;
    }

    const Value& result() const {
        return result_;
    }

private:
    using Stack = std::stack<Frame>;

    void stack_clear();
    void stack_push(const Id& id);
    void stack_pop();
    Frame& stack_top();
    bool stack_empty() const;

    const Environment& env_;
    const Id& root_;
    Params* params_;
    DataPtr data_ptr_;
    Stack stack_;

    bool is_finished_;
    Value result_;
};

}

#endif
