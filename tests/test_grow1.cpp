#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <stree/stree.hpp>

static stree::Value plus(const stree::Arguments& args, stree::DataPtr) {
    assert(args.size() == 2);
    return args[0] + args[1];
}

const stree::SymbolPtr& random_terminal(stree::Environment& env, std::random_device& rd) {
    assert(env.terminal_num() > 0);
    std::uniform_int_distribution<unsigned> dist(0, env.terminal_num() - 1);
    return env.terminal(dist(rd));
}

const stree::SymbolPtr& random_nonterminal(stree::Environment& env, std::random_device& rd) {
    assert(env.nonterminal_num() > 0);
    std::uniform_int_distribution<unsigned> dist(0, env.nonterminal_num() - 1);
    return env.nonterminal(dist(rd));
}

stree::Id grow(
    stree::Environment& env,
    int depth,
    std::random_device& rd, float p_term = 0.2)
{
    assert(depth > 0 && "Tree depth should be > 0");
    assert(env.terminal_num() > 0 && "Cannot grow a tree without terminals");
    std::uniform_real_distribution<float> dist(0, 1.0);

    stree::SymbolPtr symbol;
    if (depth == 1 || dist(rd) < p_term || env.nonterminal_num() == 0) {
        symbol = random_terminal(env, rd);
        assert(symbol->arity() == 0);
    } else {
        symbol = random_nonterminal(env, rd);
        assert(symbol->arity() > 0);
    }
    assert(symbol);
    stree::Id id = env.make_id(symbol);
    for (stree::Arity n = 0; n < id.arity(); ++n) {
        stree::Id arg = grow(env, depth - 1, rd, p_term);
        stree::id::nth_argument(env.node_manager(), id, n) = arg;
    }
    assert(!id.empty());
    return id;
}

int main() {
    using namespace std;
    using namespace stree;

    Environment env;
    env.add_positional("x", 0);
    env.add_positional("y", 1);
    env.add_function("+", 2, &::plus);

    std::random_device rd;
    int depth = 3;
    Id id1 = grow(env, depth, rd, 0.0);
    Tree t1(&env, id1);
    cout << t1 << endl;
    return 0;
}
