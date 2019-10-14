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
    env.add_function("+", 2, &func);
    env.add_function("-", 2, &func);
    env.add_function("*", 2, &func);
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
    PARSE(p1, ts1);

    // Output tree string
    cout << ts1 << endl;

    // Get tree node by node number
    Tree t1(&env, p1.move_result());
    for (NodeNum n = 0; n < answer_num; ++n) {
        cout << "[Node " << n << "] ";
        const Id& id = id::nth_node(env.node_manager(), t1.root(), n);
        const Symbol* symbol = nullptr;
        GET_SYMBOL(symbol, env, id);
        if (symbol->name() != answers[n]) {
            cout << "Primitive name `" << symbol->name() << "' doesn't match "
                 << "expected name `" << answers[n] << "'" << endl;
            return -1;
        } else {
            cout << symbol->name() << endl;
        }
    }

    return 0;
}
