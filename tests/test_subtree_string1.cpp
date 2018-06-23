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
    env.add_function("-", 2, &::func);
    env.add_function("*", 2, &::func);
    env.add_positional("a", 0);
    env.add_positional("b", 1);
    env.add_positional("c", 2);
    env.add_positional("d", 3);
    env.add_positional("e", 4);
    env.add_positional("f", 5);

    // Test data
    //                0 1  2  3 5 6   4 7 8
    std::string ts1("(* a (+ (- c d) (* e f)))");

    // Output tree string
    cout << ts1 << endl;

    // Parse
    Parser p1(&env);
    PARSE(p1, ts1);

    // Make tree
    Tree t1(&env, p1.result());
    // Output whole tree
    cout << "Tree: " << t1 << endl;

    // Output subtrees
    CHECK_SUBTREE_STR(t1, 0, ts1);
    CHECK_SUBTREE_STR(t1, 2, "(+ (- c d) (* e f))");
    CHECK_SUBTREE_STR(t1, 3, "(- c d)");
    CHECK_SUBTREE_STR(t1, 4, "(* e f)");
    CHECK_SUBTREE_STR(t1, 5, "c");
    CHECK_SUBTREE_STR(t1, 8, "f");

    return 0;
}
