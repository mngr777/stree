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
            Arguments args(id.arity());
            for (Arity n = 0; n < id.arity(); ++n)
                args[n] = eval(
                    env,
                    id::nth_argument(env.node_manager(), id, n),
                    params, data);
            return env.function(id::fid(env.node_manager(), id))(args, data);
        }
        case TypeSelect:
            assert(false && "Not implemented");
        default:
            assert(false && "Unknown type");
    }
}

}
