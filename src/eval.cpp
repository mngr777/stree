#include <stree/eval.hpp>
#include <cassert>

namespace stree {

namespace {

Arguments eval_arguments(
    const Environment& env,
    const Id& id,
    Arity arity,
    const Params& params, DataPtr data)
{
    Arguments arguments(arity);
    for (Arity n = 0; n < arity; ++n)
        arguments[n] = eval(
            env,
            id::nth_argument(env.node_manager(), id, n),
            params, data);
    return arguments;
}

}

Value eval(
    const Environment& env,
    const Id& id,
    const Params& params, DataPtr data)
{
    assert(
        id::is_valid_subtree(env.node_manager(), id)
        && "Cannot eval invalid subtree");
    switch (id.type()) {
        case TypeConst: {
            return id::value(env.node_manager(), id);
        }
        case TypePositional: {
            assert(
                id::position(env.node_manager(), id) < params.size()
                && "Invalid positional argument index");
            return params[id::position(env.node_manager(), id)];
        }
        case TypeFunction: {
            // eval arguments
            Arguments args = eval_arguments(env, id, id.arity(), params, data);
            // apply function
            return env.function(id::fid(env.node_manager(), id))(args, data);
        }
        case TypeSelect: {
            const Symbol* symbol = env.symbol(id);
            assert(symbol && "Select function symbol not found");
            Arguments args = eval_arguments(env, id, symbol->sf_arity(), params, data);
            unsigned branch = call_select_function(env, id, args, data);
            assert(branch < id.arity() && "Invalid branch selected");
            return (branch < args.size())
                ? args[branch]
                : eval(
                    env, id::nth_argument(env.node_manager(), id, branch),
                    params, data);

        }
    }
    assert(false);
}

Value eval(const Tree& tree, const Params& params, DataPtr data) {
    return eval(*tree.env(), tree.root(), params, data);
}


Value call_function(
    const Environment& env,
    const Id& id,
    const Arguments& arguments,
    DataPtr data)
{
    assert(id.arity() == arguments.size() && "Invalid argument number");
    return env.function(id::fid(env.node_manager(), id))(arguments, data);
}

unsigned call_select_function(
    const Environment& env,
    const Id& id,
    const Arguments& arguments,
    DataPtr data)
{
    SelectFunctionIndex sfid = id::sfid(env.node_manager(), id);
    assert(
        env.select_function_cond_arity(sfid) == arguments.size()
        && "Invalid argument number");
    unsigned branch = env.select_function(sfid)(arguments, data);
    assert(branch < id.arity() && "Invalid branch selected");
    return branch;
}

}
