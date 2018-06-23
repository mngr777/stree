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

#define CHECK_STATS(type, arity, pool_size_answer, buffer_size_answer)  \
    {                                                                   \
        auto item = nms1.item(type, arity);                             \
        bool valid_pool_size = (item.pool_size == pool_size_answer);    \
        bool valid_buffer_size = (item.buffer_size == buffer_size_answer); \
        if (!valid_pool_size || ! valid_buffer_size) {                  \
            cout << "[" << stree::type_to_string(type);                 \
            if (type == TypeFunction || type == TypeSelect)             \
                cout << " " << static_cast<unsigned>(arity);            \
            cout << "] "                                                \
                 << "pool size: " << item.pool_size                     \
                 << " (answer is " << pool_size_answer << "), "         \
                 << "buffer size: " << item.buffer_size                 \
                 << " (answer is " << buffer_size_answer << ")"         \
                 << endl;                                               \
            return -3;                                                  \
        }                                                               \
    }

    // Output stats
    nms1.update(env.node_manager());
    cout << "Stats after parsing" << endl;
    cout << nms1 << endl;
    CHECK_STATS(TypePositional, 0, 3, 0);
    CHECK_STATS(TypeFunction, 0, 1, 0);
    CHECK_STATS(TypeFunction, 2, 3, 0);

    {
        // Make tree
        Tree t1(&env, p1.result());
        // Get subtree
        Subtree st1 = t1.subtree(3);
        // Destroy subtree
        cout << "Destroing subtree 3: " << st1 << endl;
        st1.destroy();
        // Output updated tree
        cout << "Tree: " << t1 << endl;
        // Output stats
        nms1.update(env.node_manager());
        cout << "Stats:" << endl
             << nms1 << endl;
        CHECK_STATS(TypePositional, 0, 3, 1);
        CHECK_STATS(TypeFunction, 0, 1, 1);
        CHECK_STATS(TypeFunction, 2, 3, 1);
    } // t1 is destroyed

    // Output stats
    nms1.update(env.node_manager());
    cout << "Stats after tree destruction" << endl;
    cout << nms1 << endl;
    CHECK_STATS(TypePositional, 0, 3, 3);
    CHECK_STATS(TypeFunction, 0, 1, 1);
    CHECK_STATS(TypeFunction, 2, 3, 3);

    // Parse same string
    // This should use up buffered nodes and not create new ones
    PARSE(p1, ts1);

    nms1.update(env.node_manager());
    cout << "Stats after parsing same expression" << endl;
    cout << nms1 << endl;
    CHECK_STATS(TypePositional, 0, 3, 0);
    CHECK_STATS(TypeFunction, 0, 1, 0);
    CHECK_STATS(TypeFunction, 2, 3, 0);


#undef CHECK_STATS

    return 0;
}
