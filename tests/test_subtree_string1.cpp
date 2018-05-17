#include <cassert>
#include <iostream>
#include <string>
#include <stree/stree.hpp>

static stree::Value func(const stree::Arguments& args, stree::DataPtr) {
    return stree::Value{};
}

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
/*
        (0)*
      /     \
    (1)a    (2)+
          /      \
       (3)-      (4)*
       /  \      /  \
     (5)c (6)d (7)e (8)f
 */
    //                0 1  2  3 5 6   4 7 8
    std::string ts1("(* a (+ (- c d) (* e f)))");

    // Output tree string
    cout << ts1 << endl;

    // Parse
    Parser p1(&env);
    p1.parse(ts1);
    if (p1.is_done()) {
        // Make tree
        Tree t1(&env, p1.result());
        // Output whole tree
        cout << "Tree: " << t1 << endl;

#define CHECK_SUBTREE(n, answer)                                        \
        {                                                               \
            std::string st = stree::to_string(t1.subtree(n));           \
            cout << "Subtree " << n << ": " << st                       \
                 << ", answer: " << answer                              \
                 << endl;                                               \
            if (st != answer)                                           \
                return -3;                                              \
        }

        // Output subtrees
        CHECK_SUBTREE(0, ts1);
        CHECK_SUBTREE(2, "(+ (- c d) (* e f))");
        CHECK_SUBTREE(3, "(- c d)");
        CHECK_SUBTREE(4, "(* e f)");
        CHECK_SUBTREE(5, "c");
        CHECK_SUBTREE(8, "f");

#undef CHECK_SUBTREE

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
