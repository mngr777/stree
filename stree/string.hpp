#ifndef STREE_STRING_HPP_
#define STREE_STRING_HPP_

#include <ostream>
#include <stdexcept>
#include <stree/environment.hpp>
#include <stree/tree.hpp>

std::ostream& operator<<(std::ostream& os, const stree::Tree& tree);

namespace stree {

template<typename S>
S& id_to_stream(S& stream, const Id& id, const Environment& env) {
    if (id.empty()) {
        stream << "<empty>";
    } else if (id.type() == TypeConst) {
        stream << id::value(env.node_manager(), id);
    } else {
        auto symbol = env.symbol(id);
        if (!symbol)
            throw std::invalid_argument("Symbol not found");

        if (symbol->is_callable())
            stream << '(';
        stream << symbol->name();
        for (Arity n = 0; n < id.arity(); ++n) {
            stream << ' ';
            id_to_stream(
                stream,
                id::nth_argument(env.node_manager(), id, n),
                env);
        }
        if (symbol->is_callable())
            stream << ')';
    }
    return stream;
}


} // namespace stree

#endif
