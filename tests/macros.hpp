#ifndef ANTS_TEST_MACROS_
#define ANTS_TEST_MACROS_

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stree/stree.hpp>

// Define test function that does nothing
#define DEFUN_EMPTY(func)                                               \
    static stree::Value func(const stree::Arguments& args, stree::DataPtr) { \
        return stree::Value{};                                          \
    }

// Parse string with parser,
// print error message and exit on error
#define PARSE(parser, string)                                           \
    parser.parse(string);                                               \
    if (parser.is_error()) {                                            \
        std::cerr << "Parse error: " << p1.error_message() << std::endl; \
        std::exit(EXIT_FAILURE);                                        \
    } else if (!parser.is_done()) {                                     \
        std::cerr << "Parsing not finished" << std::endl                \
                  << "State: " << parser.state_string() << std::endl    \
                  << "Line: " << parser.line_num()                      \
                  << ", Pos: " << parser.char_num() << std::endl;       \
    }

// Check if symbol exists (by ID or name)
#define GET_SYMBOL(result, env, search)                             \
    result = env.symbol(search);                                    \
    if (!result) {                                                  \
        std::cerr << "Symbol not found: " << search << std::endl;   \
        std::exit(EXIT_FAILURE);                                    \
    }

// Check if nth subtree converted to string matches answer
#define CHECK_SUBTREE_STR(tree, n, answer)                  \
    {                                                       \
        std::string st = stree::to_string(tree.subtree(n)); \
        cout << "Subtree " << n << ": " << st               \
             << ", answer: " << answer                      \
             << endl;                                       \
        if (st != answer)                                   \
            std::exit(EXIT_FAILURE);                        \
    }

// Check if tree or subtree size matches answer
#define CHECK_SIZE(subtree, answer)                                     \
    {                                                                   \
        stree::NodeNum size = stree::id::subtree_size(env.node_manager(), t1.root()); \
        if (size != answer) {                                           \
            std::cerr << "Size does not match: "                        \
                      << size ", answer: " << answer                    \
                      << std::endl;                                     \
            std::exit(EXIT_FAILURE);                                    \
        }                                                               \
    }

#endif
