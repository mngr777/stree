#include <cassert>
#include <cstdint>
#include <iostream>
#include <stree/environment.hpp>
#include <stree/parser.hpp>
#include <stree/string.hpp>
#include <stree/tree.hpp>

static stree::Value plus(const stree::Arguments& args) {
    assert(args.size() == 2);
    return args[0] + args[1];
}

int main() {
    using namespace std;
    using namespace stree;

    Environment env;
    env.add_function("+", 2, &::plus);
    env.add_positional("x", 0);

    Parser p1(&env);
    std::string ts1("(+ x x)");
    for (const char ch : ts1) {
        p1.consume(ch);
        cout << "char: " << ch
             << ", state: " << p1.state_string() << endl;
        if (p1.is_error() || p1.is_done())
            break;
    }

    cout << ts1 << endl;
    if (p1.is_done()) {
        Tree t1(&env, p1.result());
        cout << t1 << endl;
    } else if (p1.is_error()) {
        cerr << "Parse error: " << p1.error_message() << endl;
    } else {
        cerr << "Parsing not finished" << endl
             << "State: " << p1.state_string() << endl
             << "Line: " << p1.line_num()
             << ", Pos: " << p1.char_num() << endl;
    }


    return 0;
}
