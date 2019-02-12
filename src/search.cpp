#include <stree/search.hpp>

namespace stree {

void AritySet::add(Arity arity) {
    set_.insert(arity);
}

void AritySet::remove(Arity arity) {
    set_.erase(arity);
}

bool AritySet::has(Arity arity) const {
    return set_.count(arity) == 1;
}

} // namespace stree
