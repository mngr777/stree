#include <cassert>
#include <iostream>
#include <string>
#include <stree/stree.hpp>
#include "macros.hpp"

DEFUN_EMPTY(func);

int main() {
    using namespace std;
    using namespace stree;

    // Init environment
    Environment env;
    env.add_function("+", 2, &::func);
    env.add_positional("x", 0);

    // Test data
    std::string ts1("(+ (+ (+ x x) 10.1) x)");
    const NodeNum size_answer = 7;

    // Parse
    Parser p1(&env);
    PARSE(p1, ts1);
    cout << ts1 << endl;

    // Calc. tree size
    Tree t1(&env, p1.move_result());
    cout << t1 << endl; // output tree
    NodeNum size = id::subtree_size(env.node_manager(), t1.root());
    cout << "Tree size: " << size << "; correct answer: " << size_answer << endl;
    if (size != size_answer)
        return -1;


    return 0;
}
