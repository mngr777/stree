#ifndef STREE_STRING_HPP_
#define STREE_STRING_HPP_

#include <ostream>
#include <stdexcept>
#include <string>
#include <stree/environment.hpp>
#include <stree/tree.hpp>

std::ostream& operator<<(std::ostream& os, const stree::Tree& tree);
std::ostream& operator<<(std::ostream& os, const stree::Subtree& subtree);

namespace stree {

std::string to_string(const Id& id);
std::string to_string(const stree::Tree& tree);
std::string to_string(const stree::Subtree& subtree);

std::string subtree_to_string(const Id& id, const Environment& env);

template<typename S>
S& subtree_to_stream(S& stream, const Id& id, const Environment& env) {
    if (id.empty()) {
        stream << "<empty>";
    } else if (id.type() == TypeConst) {
        stream << id::value(env.node_manager(), id);
    } else {
        auto symbol = env.symbols().by_id(id);
        if (!symbol)
            throw std::invalid_argument(
                std::string("Symbol not found, id: ") + to_string(id));

        if (symbol->is_callable())
            stream << '(';
        stream << symbol->name();
        for (Arity n = 0; n < id.arity(); ++n) {
            stream << ' ';
            subtree_to_stream(
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
