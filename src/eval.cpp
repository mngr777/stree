#include <stree/eval.hpp>
#include <cassert>

namespace stree {

Value eval(
    const Environment& env,
    const Id& id,
    const Params& params, DataPtr data)
{
    assert(
        id::is_valid_subtree(env.node_manager(), id)
        && "Cannot eval invalid subtree");
    switch (id.type()) {
        case TypeConst:
            return id::value(env.node_manager(), id);

        case TypePositional:
            assert(id::position(env.node_manager(), id) < params.size());
            return params[id::position(env.node_manager(), id)];

        case TypeFunction: {
            // eval arguments
            Arguments args(id.arity());
            for (Arity n = 0; n < id.arity(); ++n)
                args[n] = eval(
                    env,
                    id::nth_argument(env.node_manager(), id, n),
                    params, data);
            // apply function
            return env.function(id::fid(env.node_manager(), id))(args, data);
        }

        case TypeSelect: {
            // eval conditional arguments
            SelectFunctionIndex sfid = id::sfid(env.node_manager(), id);
            Arity cond_arity = env.select_function_cond_arity(sfid);
            Arguments args(cond_arity);
            for (Arity n = 0; n < cond_arity; ++n)
                args[n] = eval(
                    env,
                    id::nth_argument(env.node_manager(), id, n),
                    params, data);
            // apply select function
            unsigned branch = env.select_function(sfid)(args, data);
            // eval selected branch
            assert(branch < id.arity());
            return eval(
                env,
                id::nth_argument(env.node_manager(), id, branch),
                params, data);
        }

        default: assert(false);
    }
}

Value eval(const Tree& tree, const Params& params, DataPtr data) {
    return eval(*tree.env(), tree.root(), params, data);
}

}
