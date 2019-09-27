#include <iostream>
#include <random>
#include <string>
#include <stree/stree.hpp>
#include "macros.hpp"

DEFUN_EMPTY(func)

using namespace std;
using namespace stree;

// TODO: move selecting random symbol to environment (with prng as argument)

template<typename R>
const Symbol* random_term(Environment& env, R& prng) {
    unsigned term_num = env.terminal_num();
    assert(term_num > 1);
    std::uniform_int_distribution<unsigned> dist(0, term_num - 1);
    return env.terminal(dist(prng));
}

template<typename R>
const Symbol* random_nonterm(Environment& env, R& prng) {
    unsigned nonterm_num = env.nonterminal_num();
    assert(nonterm_num > 1);
    std::uniform_int_distribution<unsigned> dist(0, nonterm_num - 1);
    return env.nonterminal(dist(prng));
}

template<typename R>
bool cointoss(R& prng, float p_true) {
    return std::bernoulli_distribution{p_true}(prng);
}

// NOTE: assuming # of non-terminals is > 0
template<typename R>
void grow(Builder& builder, unsigned depth, R& prng, float p_term = 0.2) {
    if (depth > 1 && !cointoss(prng, p_term)) {
        // Set random nonterminal
        const Symbol* symbol = random_nonterm(builder.env(), prng);
        assert(symbol);
        builder.set(symbol);
        // Grow argument subtrees
        for (unsigned n = 0; n < symbol->arity(); ++n) {
            builder.down(n);
            grow(builder,  depth - 1, prng, p_term);
            builder.up();
        }
    } else {
        // Set random terminal
        builder.set(random_term(builder.env(), prng));
    }
}

template<typename R>
void full(Builder& builder, unsigned depth, R& prng) {
    unsigned nonterm_num = builder.env().nonterminal_num();
    if (depth > 1 && nonterm_num > 1) {
        // Set random nonterminal
        const Symbol* symbol = random_nonterm(builder.env(), prng);
        assert(symbol);
        builder.set(symbol);
        // Build children
        for (unsigned n = 0; n < symbol->arity(); ++n) {
            builder.down(n);
            full(builder, depth - 1, prng);
            builder.up();
        }
    } else {
        // Set random terminal
        builder.set(random_term(builder.env(), prng));
    }
}


int main() {


    Environment env;
    env.add_function("+", 2, &::func);
    env.add_function("-", 2, &::func);
    env.add_function("*", 2, &::func);
    env.add_function("%", 2, &::func);
    env.add_positional("a", 0);
    env.add_positional("b", 1);
    env.add_positional("c", 2);

    const unsigned depth = 5;
    const unsigned prng_seed = 1;
    std::mt19937 prng(prng_seed);

    Builder b1(env);
    {
        grow(b1, depth, prng);
        Tree t_grow1 = Tree(&env, b1.root());
        bool is_valid = (b1.is_valid());
        b1.reset();
        cout << "Generated tree (grow): " << t_grow1 << endl;
        if (!is_valid) {
            cerr << "Invalid tree!" << endl;
            return -3;
        }
    }
    {
        full(b1, depth, prng);
        bool is_valid = (b1.is_valid());
        Tree t_full1 = Tree(&env, b1.root());
        b1.reset();
        cout << "Generated tree (full): " << t_full1 << endl;
        if (!is_valid) {
            cerr << "Invalid tree!" << endl;
            return -3;
        }
    }


    return 0;
}
