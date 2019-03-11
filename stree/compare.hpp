#ifndef STREE_COMPARE_HPP_
#define STREE_COMPARE_HPP_

namespace stree {

class Id;
class Environment;

class NodeCompare {
public:
    NodeCompare(Environment* env)
        : env_(env),
          compare_arity_(true),
          compare_symbols_(false) {}

    bool equal(const Id& id1, const Id& id2) const;

private:
    bool match_arity(const Id& id1, const Id& id2) const;
    bool match_symbols(const Id& id1, const Id& id2) const;

    Environment* env_;
    bool compare_arity_;
    bool compare_symbols_;
};

} // namespace stree

#endif
