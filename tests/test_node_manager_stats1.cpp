#include <iostream>
#include <string>
#include <stree/stree.hpp>
#include "macros.hpp"
#include "node_manager_macros.hpp"

DEFUN_EMPTY(func);

int main() {
    using namespace std;
    using namespace stree;

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
    //                0 1  2  3  5  6  4
    std::string ts1("(+ a (- (* (u) c) b))");

    // Create stats object
    stree::NodeManagerStats nms1;

    // Parse
    Parser p1(&env);
    PARSE(p1, ts1);

    // Output tree string
    cout << ts1 << endl;

    // Output stats
    nms1.update(env.node_manager());
    cout << "Stats after parsing" << endl;
    cout << nms1 << endl;
    CHECK_STATS(nms1, TypePositional, 0, 3, 0);
    CHECK_STATS(nms1, TypeFunction, 0, 1, 0);
    CHECK_STATS(nms1, TypeFunction, 2, 3, 0);

    {
        // Make tree
        Tree t1(&env, p1.result());
        // Get subtree
        Subtree st1 = t1.sub(3);
        // Destroy subtree
        cout << "Destroing subtree 3: " << st1 << endl;
        st1.destroy();
        // Output updated tree
        cout << "Tree: " << t1 << endl;
        // Output stats
        nms1.update(env.node_manager());
        cout << "Stats:" << endl
             << nms1 << endl;
        CHECK_STATS(nms1, TypePositional, 0, 3, 1);
        CHECK_STATS(nms1, TypeFunction, 0, 1, 1);
        CHECK_STATS(nms1, TypeFunction, 2, 3, 1);
    } // t1 is destroyed

    // Output stats
    nms1.update(env.node_manager());
    cout << "Stats after tree destruction" << endl;
    cout << nms1 << endl;
    CHECK_STATS(nms1, TypePositional, 0, 3, 3);
    CHECK_STATS(nms1, TypeFunction, 0, 1, 1);
    CHECK_STATS(nms1, TypeFunction, 2, 3, 3);

    // Parse same string
    // This should use up buffered nodes and not create new ones
    PARSE(p1, ts1);

    nms1.update(env.node_manager());
    cout << "Stats after parsing same expression" << endl;
    cout << nms1 << endl;
    CHECK_STATS(nms1, TypePositional, 0, 3, 0);
    CHECK_STATS(nms1, TypeFunction, 0, 1, 0);
    CHECK_STATS(nms1, TypeFunction, 2, 3, 0);

    return 0;
}
