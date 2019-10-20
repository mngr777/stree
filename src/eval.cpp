#include <stree/eval.hpp>
#include <cassert>

namespace stree {

namespace {

Arguments eval_arguments(
    const Environment& env,
    const Id& id,
    const Params& params, DataPtr data)
{
    Arity argument_num = get_argument_num(env, id);
    Arguments arguments(argument_num);
    for (Arity n = 0; n < argument_num; ++n)
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
            Arguments args = eval_arguments(env, id, params, data);
            // apply function
            return call_function(env, id, args, data);
        }
        case TypeSelect: {
            Arguments args = eval_arguments(env, id, params, data);
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


Arity get_argument_num(const Environment& env, const Id& id) {
    Arity n = id.arity();
    if (id.type() == TypeSelect) {
        const SymbolPtr& symbol = env.symbols().by_id(id);
        assert(symbol && "Select symbol not found");
        n = symbol->sf_arity();
    }
    return n;
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
        env.symbols().by_sfid(sfid)->sf_arity() == arguments.size()
        && "Invalid argument number");
    unsigned branch = env.select_function(sfid)(arguments, data);
    assert(branch < id.arity() && "Invalid branch selected");
    return branch;
}

}
