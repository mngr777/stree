#include <cassert>
#include <iostream>
#include <string>
#include <stree/stree.hpp>

static stree::Value func(const stree::Arguments& args, stree::DataPtr) {
    return 0;
}

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
    p1.parse(ts1);
    if (p1.is_done()) {
        // Make tree
        Tree t1(&env, p1.result());
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
            const Symbol* symbol = env.symbol(id);
            if (!symbol) {
                cout << "Original: symbol not found by ID " << id << endl;
                return -3;
            }

            // Copy
            const Id& copy_id = id::nth_node(env.node_manager(), t2.root(), n);
            const Symbol* copy_symbol = env.symbol(copy_id);
            if (!copy_symbol) {
                cout << "Copy: symbol not found by ID" << copy_id << endl;
                return -4;
            }

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

    } else if (p1.is_error()) {
        cerr << "Parse error: " << p1.error_message() << endl;
        return -1;
    } else {
        cerr << "Parsing not finished" << endl
             << "State: " << p1.state_string() << endl
             << "Line: " << p1.line_num()
             << ", Pos: " << p1.char_num() << endl;
        return -2;
    }
    return 0;
}
