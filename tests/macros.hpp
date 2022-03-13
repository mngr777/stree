#ifndef STREE_TEST_MACROS_
#define STREE_TEST_MACROS_

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include <stree/stree.hpp>

// Define test function that does nothing
#define DEFUN_EMPTY(func)                                               \
    static stree::Value func(const stree::Arguments& args, stree::DataPtr) { \
        return stree::Value{};                                          \
    }

// Define test function that passes first argument through
#define DEFUN_PASS(func)                                                \
    static stree::Value func(const stree::Arguments& args, stree::DataPtr) { \
        assert(!args.empty());                                          \
        return args[0];                                                 \
    }

// Define progn function (return last argument or zero)
#define DEFUN_PROGN(func)                                               \
    static stree::Value func(const stree::Arguments& args, stree::DataPtr) { \
        return !args.empty()                                            \
            ? static_cast<stree::Value>(args.back())                    \
            : stree::Value{};                                           \
    }

// Parse string with parser,
// print error message and exit on error
#define PARSE(parser, string)                                           \
    parser.parse(string);                                               \
    if (parser.is_error())                                              \
        std::cerr << "Parse error: " << parser.error_message() << std::endl; \
    if (!parser.is_done()) {                                            \
        std::cerr << string << std::endl;                               \
        std::cerr << "Parsing not finished" << std::endl                \
                  << "State: " << parser.state_string() << std::endl    \
                  << "Line: " << parser.line_num()                      \
                  << ", Pos: " << parser.char_num() << std::endl        \
                  << ", Buffer: " << parser.buffer() << std::endl;      \
        std::exit(EXIT_FAILURE);                                        \
    }

#define CHECK_TREE_STR(tree, answer)                                \
    {                                                               \
        std::string st = stree::to_string(tree);                    \
        cout << "Tree: " << st << ", answer: " << answer << endl;   \
        if (st != answer)                                           \
            std::exit(EXIT_FAILURE);                                \
    }

// Check if nth subtree converted to string matches answer
#define CHECK_SUBTREE_STR(tree, n, answer)                  \
    {                                                       \
        std::string st = stree::to_string(tree.sub(n));     \
        cout << "Subtree " << n << ": " << st               \
             << ", answer: " << answer                      \
             << endl;                                       \
        if (st != answer)                                   \
            std::exit(EXIT_FAILURE);                        \
    }

// Check if tree evaluates to correct value
#define CHECK_EVAL(tree, params, answer, data_ptr)                      \
    {                                                                   \
        stree::Value value = stree::eval(tree, params, data_ptr);       \
        cout << "result: " << value << ", answer: " << answer << endl;  \
        if (value != answer)                                            \
            std::exit(EXIT_FAILURE);                                    \
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
