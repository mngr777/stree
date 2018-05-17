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
    p1.parse(ts1);

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

    // Test
    if (p1.is_done()) {
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

    // Output stats
    nms1.update(env.node_manager());
    cout << "Stats after tree destruction" << endl;
    cout << nms1 << endl;
    CHECK_STATS(TypePositional, 0, 3, 3);
    CHECK_STATS(TypeFunction, 0, 1, 1);
    CHECK_STATS(TypeFunction, 2, 3, 3);

#undef CHECK_STATS

    return 0;
}
