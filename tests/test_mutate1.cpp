#include <iostream>
#include <string>
#include <stree/stree.hpp>
#include "macros.hpp"

static stree::Value plus(const stree::Arguments& args, stree::DataPtr) {
    assert(args.size() == 2);
    return args[0] + args[1];
}

static stree::Value mult(const stree::Arguments& args, stree::DataPtr) {
    assert(args.size() == 2);
    return args[0] * args[1];
}

int main() {
    using namespace std;
    using namespace stree;

    Environment env;
    env.add_positional("a", 0);
    env.add_positional("b", 1);
    env.add_function("+", 2, &::plus);
    env.add_function("*", 2, &::mult);

    Parser parser(&env);
    Params params{2, 3};

    // Make tree
    string ts1("(+ a b)");
    PARSE(parser, ts1);
    Tree t1(&env, parser.result());
    cout << t1 << endl;

    // Mutate
    t1.set("*");
    // Check tree string
    cout << t1 << endl;
    CHECK_SUBTREE_STR(t1, 0, "(* a b)");
    // Check eval result
    CHECK_EVAL(t1, params, 6);

    // Mutate
    t1.sub(1).set("b");
    // Check tree string
    CHECK_SUBTREE_STR(t1, 0, "(* b b)");
    // Check eval result
    CHECK_EVAL(t1, params, 9);

    return 0;
}
