#include <iostream>
#include <string>
#include <utility>
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
    env.add_function("%", 2, &func);
    env.add_positional("a", 0);
    env.add_positional("b", 1);
    env.add_positional("c", 2);

    // Test data
    string ts1("(+ (* a b) (% c 1))");

    // Create stats object
    NodeManagerStats nms1;
    NodeManagerStats nms2;

    // Parse
    Parser p1(&env);
    PARSE(p1, ts1);

    // Output stats
    nms1.update(env.node_manager());
    cout << "Stats after parsing" << endl;
    cout << nms1 << endl;

    // Check stats
    CHECK_STATS(nms1, TypeConst, 0, 1, 0);
    CHECK_STATS(nms1, TypePositional, 0, 3, 0);
    CHECK_STATS(nms1, TypeFunction, 2, 3, 0);

    // Create empty trees
    Tree t1(&env);
    Tree t2(&env);

    // Output stats
    nms2.update(env.node_manager());
    cout << "Stats after creating empty tree" << endl;
    cout << nms2 << endl;

    // Check if stats match
    if (nms2 != nms1) {
        cerr << "Stats do not match" << endl;
        return -3;
    }

    {
        // Make tree
        Tree t3(&env, p1.move_result());

        // Copy tree
        t1 = t3;
        // Output stats
        nms1.update(env.node_manager());
        cout << "Stats after copying" << endl;
        cout << nms1 << endl;
        // Check stats
        CHECK_STATS(nms1, TypeConst, 0, 2, 0);
        CHECK_STATS(nms1, TypePositional, 0, 6, 0);
        CHECK_STATS(nms1, TypeFunction, 2, 6, 0);

        // Move tree
        t2 = move(t3);
        // Output stats
        nms2.update(env.node_manager());
        cout << "Stats after moving" << endl;
        cout << nms2 << endl;
        // Check stats
        if (nms2 != nms1) {
            cerr << "Stats do not match" << endl;
            return -4;
        }
    }

    // Output stats
    nms1.update(env.node_manager());
    cout << "Stats after moved object destruction" << endl;
    cout << nms1 << endl;
    if (nms1 != nms2) {
        cerr << "Stats do not match" << endl;
        return -5;
    }

    return 0;
}
