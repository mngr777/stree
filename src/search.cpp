#include <stree/search.hpp>
#include <stree/node.hpp>

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

bool AritySet::empty() const {
    return set_.empty();
}


bool NodeFilter::match(const Id& id) const {
    return terminality_match(id) && arity_match(id);
}

bool NodeFilter::empty() const {
    return is_terminal == IsTerminalAny && arity.empty();
}

bool NodeFilter::terminality_match(const Id& id) const {
    return is_terminal == IsTerminalAny
        || (is_terminal == IsTerminalYes) == (id.arity() == 0);
}

bool NodeFilter::arity_match(const Id& id) const {
    return arity.empty() || arity.has(id.arity());
}

} // namespace stree
