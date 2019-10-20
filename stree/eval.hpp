#ifndef STREE_EVAL_HPP_
#define STREE_EVAL_HPP_

#include <vector>
#include <stree/environment.hpp>
#include <stree/tree.hpp>

namespace stree {

using Params = std::vector<Value>;

Value eval(
    const Environment& env,
    const Id& id,
    const Params& params, DataPtr data = nullptr);

Value eval(const Tree& tree, const Params& params, DataPtr data = nullptr);

// TODO: refactoring ??
Arity get_argument_num(const Environment& env, const Id& id);

Value call_function(
    const Environment& env,
    const Id& id,
    const Arguments& arguments,
    DataPtr data = nullptr);

unsigned call_select_function(
    const Environment& env,
    const Id& id,
    const Arguments& arguments,
    DataPtr data = nullptr);

}

#endif
