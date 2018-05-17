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

        // Output subtrees
        std::string st0 = stree::to_string(t1.subtree(0));
        std::string answer0 = ts1;
        cout << "Subtree 0: " << st0 << ", correct answer: " << answer0 << endl;
        if (st0 != answer0)
            return -3;

        std::string st2 = stree::to_string(t1.subtree(2));
        std::string answer2("(+ (- c d) (* e f))");
        cout << "Subtree 2: " << st2 << ", correct answer: " << answer2 << endl;
        if (st2 != answer2)
            return -3;

        std::string st3 = stree::to_string(t1.subtree(3));
        std::string answer3("(- c d)");
        cout << "Subtree 3: " << st3 << ", correct answer: " << answer3 << endl;
        if (st3 != answer3)
            return -3;

        std::string st4 = stree::to_string(t1.subtree(4));
        std::string answer4("(* e f)");
        cout << "Subtree 4: " << st4 << ", correct answer: " << answer4 << endl;
        if (st4 != answer4)
            return -3;

        std::string st5 = stree::to_string(t1.subtree(5));
        std::string answer5("c");
        cout << "Subtree 5: " << st5 << ", correct answer: " << answer5 << endl;
        if (st5 != answer5)
            return -3;

        std::string st8 = stree::to_string(t1.subtree(8));
        std::string answer8("f");
        cout << "Subtree 8: " << st8 << ", correct answer: " << answer8 << endl;
        if (st8 != answer8)
            return -3;

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
