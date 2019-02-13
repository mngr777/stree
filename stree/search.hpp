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
    bool empty() const;

private:
    std::unordered_set<Arity> set_;
};


class Id;

class NodeFilter {
public:
    NodeFilter()
        : is_terminal(IsTerminalAny) {}

    bool match(const Id& id) const;
    bool empty() const;

    IsTerminal is_terminal;
    AritySet arity;

private:
    bool terminality_match(const Id& id) const;
    bool arity_match(const Id& id) const;
};

} // namespace stree

#endif
