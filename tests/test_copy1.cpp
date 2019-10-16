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
    env.add_positional("x", 0);
    env.add_positional("y", 1);

    // Test data
    /*
        (+)
        /  \
      (x)  (-)
           /  \
         (x)  (y)
     */
    std::string ts1("(+ x (- x y))");

    // Parse
    Parser p1(&env);
    PARSE(p1, ts1);

    // Make tree
    Tree t1(&env, p1.move_result());
    cout << t1 << endl; // output tree
    // Calc. tree size
    auto t1_size = id::subtree_size(env.node_manager(), t1.root());

    // Copy tree
    Tree t2(&env, id::copy_subtree(env.node_manager(), t1.root()));
    cout << t2 << endl; // output tree copy
    // Calc. tree copy size
    auto t2_size = id::subtree_size(env.node_manager(), t2.root());

    // Check if sizes match
    cout << "Tree sizes:" << endl
         << "original: " << t1_size << ", copy: "  << t2_size << endl;
    if (t1_size != t2_size) {
        return -3;
    }

    // Check if node names match
    for (NodeNum n = 0; n < t1_size; ++n) {
        // Original
        const Id& id = id::nth_node(env.node_manager(), t1.root(), n);
        SymbolPtr symbol;
        GET_SYMBOL(symbol, env, id);

        // Copy
        const Id& copy_id = id::nth_node(env.node_manager(), t2.root(), n);
        SymbolPtr copy_symbol;
        GET_SYMBOL(copy_symbol, env, copy_id);

        // Output
        cout << "[Node " << n << "] "
             << "original: " << symbol->name()
             << ", copy: " << copy_symbol->name() << endl;

        // Check copy and original node ID addresses do not match
        if (&id == &copy_id) {
            cout << "Original and copy IDs have same addresses" << endl;
            return -5;
        }

        // Check if symbols match
        if (*symbol != *copy_symbol) {
            cout << "Node symbols do not match:" << endl
                 << "Original: `" << symbol->name() << "', "
                 << "copy: `" << copy_symbol->name() << "'" << endl;
            return -6;
        }
    }

    return 0;
}
