#include <cassert>
#include <iostream>
#include <string>
#include <stree/stree.hpp>

static stree::Value plus(const stree::Arguments& args, stree::DataPtr) {
    return 0;
}

static stree::Value minus(const stree::Arguments& args, stree::DataPtr) {
    return 0;
}

static stree::Value mult(const stree::Arguments& args, stree::DataPtr) {
    return 0;
}

int main() {
    using namespace std;
    using namespace stree;

    // Init environment
    Environment env;
    env.add_function("+", 2, &::plus);
    env.add_function("-", 2, &::minus);
    env.add_function("*", 2, &::mult);
    env.add_positional("a", 1);
    env.add_positional("b", 2);
    env.add_positional("c", 3);
    env.add_positional("d", 4);

    // Test data
/*
     (0)+
     /  \
   (1)a (2)-
        /  \
      (3)* (4)b
      /  \
    (5)c (6)d
*/
    //                0 1  2  3 5 6  4
    std::string ts1("(+ a (- (* c d) b))");
    std::string answers[] = {"+", "a", "-", "*", "b", "c", "d"};
    NodeNum answer_num = 7;

    // Parse
    Parser p1(&env);
    p1.parse(ts1);

    // Output tree
    cout << ts1 << endl;

    // Test
    if (p1.is_done()) {
        // Get tree node by node number
        Tree t1(&env, p1.result());
        for (NodeNum n = 0; n < answer_num; ++n) {
            cout << "[Node " << n << "] ";
            const Id& id = id::nth_node(env.node_manager(), t1.root(), n);
            const Symbol* symbol = env.symbol(id);
            if (!symbol) {
                cout << "Symbol not found by ID " << id << endl;
                return -1;
            } else if (symbol->name() != answers[n]) {
                cout << "Primitive name `" << symbol->name() << "' doesn't match "
                     << "expected name `" << answers[n] << "'" << endl;
                return -2;
            } else {
                cout << symbol->name() << endl;
            }
        }
    } else if (p1.is_error()) {
        cerr << "Parse error: " << p1.error_message() << endl;
        return -3;
    } else {
        cerr << "Parsing not finished" << endl
             << "State: " << p1.state_string() << endl
             << "Line: " << p1.line_num()
             << ", Pos: " << p1.char_num() << endl;
        return -4;
    }
    return 0;
}
