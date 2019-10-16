#include <stree/compare.hpp>
#include <stree/node.hpp>
#include <stree/environment.hpp>

namespace stree {

bool NodeCompare::equal(const Id& id1, const Id& id2) const {
    return match_arity(id1, id2)
        && match_symbols(id1, id2);
}

bool NodeCompare::match_arity(const Id& id1, const Id& id2) const {
    return (!compare_arity_ && (id1.arity() == 0) == (id2.arity() == 0))
        || (id1.arity() == id2.arity());
}

bool NodeCompare::match_symbols(const Id& id1, const Id& id2) const {
    return !compare_symbols_
        || (env_->symbols().by_id(id1) == env_->symbols().by_id(id2));
}

} // namespace stree
