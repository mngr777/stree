#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <stree/stree.hpp>
#include "macros.hpp"

using namespace std;
using namespace stree;

DEFUN_EMPTY(func)

int main() {

    // Init environment
    Environment env;
    env.add_function("+", 2, &func);
    env.add_function("-", 2, &func);
    env.add_positional("a", 1);
    env.add_positional("b", 2);
    env.add_positional("c", 3);

    string ts1("(+ a b)");
    string ts2("(- a (+ b c))");
    string ts3("(- b c)");
    stringstream tss1;
    tss1 << "  "
         << ts1
         << "\n  "
         << ts2
         << " \n\n\t"
         << ts3
         << "  ";

    Parser parser(&env);
    std::vector<Tree> trees1;
    do {
        parser.parse(tss1);

        if (parser.is_done()) {
            trees1.emplace_back(&env, parser.move_result());
        } else {
            if (parser.is_ready())
                break; // nothing to parse

            if (parser.is_error())
                cerr << "Parse error: " << parser.error_message() << endl;

            cerr << "Parsing not finished" << endl
                 << "State: " << parser.state_string() << endl
                 << "Line: " << parser.line_num()
                 << ", Pos: " << parser.char_num() << endl
                 << ", Buffer: " << parser.buffer() << endl;
            return -1;
        }
    } while (true);

    // Check result
    if (trees1.size() != 3) {
        cerr << "Wrong number of trees: " << trees1.size() << ", answer: 3" << endl;
        return -1;
    }
    CHECK_TREE_STR(trees1[0], ts1);
    CHECK_TREE_STR(trees1[1], ts2);
    CHECK_TREE_STR(trees1[2], ts3);

}
