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
    // terminals:       0       2 3  1
    // non-terinals:  0    1  2
    std::string ts1("(+ a (- (* c d) b))");
    std::string answers_term[] = {"a", "b", "c", "d"};
    NodeNum answers_term_num = 4;
    std::string answers_nonterm[] = {"+", "-", "*"};
    NodeNum answers_nonterm_num = 3;

    // Parse
    Parser p1(&env);
    PARSE(p1, ts1);

    // Output tree string
    cout << ts1 << endl;

    // Make tree
    Tree t1(&env, p1.move_result());

    // Terminals
    cout << "Terminals" << endl;
    for (NodeNum n = 0; n < answers_term_num; ++n) {
        cout << "[Node " << n << "] ";
        NodeFilter filter;
        filter.is_terminal = IsTerminalYes;
        const Id& id = id::nth_node(env.node_manager(), t1.root(), n, filter);
        const Symbol* symbol = nullptr;
        GET_SYMBOL(symbol, env, id);
        if (symbol->name() != answers_term[n]) {
            cout << "Terminal name `" << symbol->name() << "' doesn't match "
                 << "expected name `" << answers_term[n] << "'" << endl;
            return -1;
        } else {
            cout << symbol->name() << endl;
        }
    }

    // Non-terminals
    cout << "Non-terminals" << endl;
    for (NodeNum n = 0; n < answers_nonterm_num; ++n) {
        cout << "[Node " << n << "] ";
        NodeFilter filter;
        filter.is_terminal = IsTerminalNo;
        const Id& id = id::nth_node(env.node_manager(), t1.root(), n, filter);
        const Symbol* symbol = nullptr;
        GET_SYMBOL(symbol, env, id);
        if (symbol->name() != answers_nonterm[n]) {
            cout << "Non-terminal name `" << symbol->name() << "' doesn't match "
                 << "expected name `" << answers_nonterm[n] << "'" << endl;
            return -1;
        } else {
            cout << symbol->name() << endl;
        }
    }

    return 0;
}
