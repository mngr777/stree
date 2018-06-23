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
    env.add_function("%", 2, &::func);
    env.add_positional("a", 0);
    env.add_positional("b", 1);
    env.add_positional("c", 2);
    env.add_positional("d", 3);
    env.add_positional("e", 4);
    env.add_positional("f", 5);

    // Test data
/*
      (+)0
      /  \
    (*)1 (a)2
    /  \
  (b)3 (-)4
       /  \
     (c)5  (d)6
*/
    string ts1("(+ (* b (- c d)) a)");
/*
      (%)0
      /  \
    (e)1 (f)2
*/
    string ts2("(% e f)");

    // Parse
    Parser p1(&env);
    // tree 1
    PARSE(p1, ts1);
    Tree t1(&env, p1.result());
    // tree 2
    PARSE(p1, ts2);
    Tree t2(&env, p1.result());

    // Output trees
    cout << t1 << endl;
    cout << t2 << endl;

    // Store stats
    NodeManagerStats nms1;
    nms1.update(env.node_manager());

    // Output stats
    cout << "Stats before" << endl
         << nms1 << endl;

    // Swap subtrees
    t1.subtree(3).swap(t2.subtree(0));

    // Store new stats
    NodeManagerStats nms2;
    nms2.update(env.node_manager());

    // Output stats
    cout << "Stats after" << endl
         << nms2 << endl;

    // Compare stats
    if (nms2 != nms1) {
        cerr << "stats do not match" << endl;
        return -1;
    }

    return 0;
}
