#include <cassert>
#include <iostream>
#include <string>
#include <stree/stree.hpp>

static stree::Value plus(const stree::Arguments& args, stree::DataPtr) {
    return 0;
}

int main() {
    using namespace std;
    using namespace stree;

    // Init environment
    Environment env;
    env.add_function("+", 2, &::plus);
    env.add_positional("x", 0);

    // Test data
    std::string ts1("(+ (+ (+ x x) 10.1) x)");
    const NodeNum size_answer = 7;

    // Parse
    Parser p1(&env);
    p1.parse(ts1);
    cout << ts1 << endl;
    if (p1.is_done()) {
        // Calc. tree size
        Tree t1(&env, p1.result());
        cout << t1 << endl; // output tree
        NodeNum size = id::subtree_size(env.node_manager(), t1.root());
        cout << "Tree size: " << size << "; correct answer: " << size_answer << endl;
        if (size != size_answer)
            return -1;

    } else if (p1.is_error()) {
        cerr << "Parse error: " << p1.error_message() << endl;
        return -2;
    } else {
        cerr << "Parsing not finished" << endl
             << "State: " << p1.state_string() << endl
             << "Line: " << p1.line_num()
             << ", Pos: " << p1.char_num() << endl;
        return -3;
    }
    return 0;
}
