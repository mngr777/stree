#ifndef STREE_SEARCH_HPP_
#define STREE_SEARCH_HPP_

#include <unordered_set>
#include <stree/types.hpp>

namespace stree {

// Term/nonterm
enum IsTerminal {
    IsTerminalYes,
    IsTerminalNo,
    IsTerminalAny
};


class AritySet {
public:
    void add(Arity arity);
    void remove(Arity arity);
    bool has(Arity arity) const;
private:
    std::unordered_set<Arity> set_;
};


struct NodeFilter {
    NodeFilter()
        : is_terminal(IsTerminalAny) {}

    IsTerminal is_terminal;
    AritySet arity;
};

} // namespace stree

#endif
