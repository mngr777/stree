#include <iostream>
#include <string>
#include <stree/stree.hpp>
#include "macros.hpp"

using namespace std;
using namespace stree;

DEFUN_EMPTY(func)

// NOTE: parser going out of scope should not invalidate result
Tree parse(Environment& env, const string& s, NodeManagerStats& stats) {
    Parser parser(&env);
    PARSE(parser, s);
    stats.update(env.node_manager());
    return Tree(&env, parser.move_result());
}

int main() {

    // Init environment
    Environment env;
    env.add_function("+", 2, &func);
    env.add_function("-", 2, &func);
    env.add_function("*", 2, &func);
    env.add_function("u", 0, &func);
    env.add_positional("a", 1);
    env.add_positional("b", 2);
    env.add_positional("c", 3);

    // Test data
/*
     (0)+
     /  \
   (1)a (2)-
        /  \
      (3)* (4)b
      /  \
    (5)u (6)c
*/
    string ts1("(+ a (- (* (u) c) b))");

    // Parse
    NodeManagerStats nms1, nms2;
    Tree t1(parse(env, ts1, nms1));
    if (!t1.is_valid()) {
        cerr << "Parsed tree is invalid" << endl;
        return -1;
    }
    nms2.update(env.node_manager());

    // Print stats
    cout << "Node manager stats after parsing:" << endl
         << nms1 << endl;
    cout << "Node manager stats after tree is returned:" << endl
         << nms2 << endl;

    if (nms2 != nms1) {
        cerr << "Stats do not match" << endl;
        return -1;
    }

    return 0;
}
