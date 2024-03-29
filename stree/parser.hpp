#ifndef STREE_PARSER_HPP_
#define STREE_PARSER_HPP_

#include <cassert>
#include <cstddef>
#include <istream>
#include <stack>
#include <stdexcept>
#include <string>
#include <stree/environment.hpp>
#include <stree/tree.hpp>

namespace stree {

class Parser;

class ParserError : std::exception {
public:
    ParserError(const Parser& parser);

    virtual const char* what() const noexcept {
        return what_.c_str();
    }
private:
    std::string what_;
};


class Parser {
public:
    enum State {
        StateReady,
        StateExpectCallableSymbol,
        StateVariableSymbol,
        StateCallableSymbol,
        StateCallableArguments,
        StateNumber,
        StateError,
        StateDone
    };

    enum Error {
        ErrorOk,
        ErrorEmpty,
        ErrorUnexpectedEnd,
        ErrorInvalidChar,
        ErrorUnexpectedLeftParen,
        ErrorUnexpectedRightParen,
        ErrorUnexpectedNumber,
        ErrorUnexpectedNonNumber,
        ErrorSymbolNotFound,
        ErrorSymbolUnexpectedVariable,
        ErrorSymbolUnexpectedCallable,
        ErrorTooManyArguments,
        ErrorNotEnoughArguments,
        ErrorNumberInvalid,
        ErrorNumberOutOfRange,
        ErrorNumberUnexpectedDot
    };

    // TODO: copy, assignment
    Parser(Environment* env);
    ~Parser();

    std::size_t parse(const std::string& s);
    std::size_t parse(std::istream& s);

    void consume(const char c);

    void finish();

    Id move_result();

    Id result() const {
        return root_;
    }

    bool is_ready() const {
        return state_ == StateReady;
    }

    bool is_done() const {
        return state_ == StateDone;
    }

    bool is_error() const {
        return state_ == StateError;
    }

    Error error() const{
        return error_;
    }

    std::string state_string() const;
    std::string error_message() const;

    std::size_t line_num() const {
        return line_num_;
    }

    std::size_t char_num() const {
        return char_num_;
    }

    const std::string& buffer() const {
        return buffer_;
    }

private:
    struct Frame {
        Frame(Id id) : id(id), child_num(0) {}
        Id id;
        unsigned child_num; // current child number
    };
    typedef std::stack<Frame> Stack;

    void reset();
    void cleanup();
    void count(char c);

    void space();
    void paren_left();
    void paren_right();
    void numeric(char c);
    void dot();
    void ident(char c);

    void complete_variable();
    void complete_callable_symbol();
    void complete_callable();
    void complete_number();
    void complete_symbol(const SymbolPtr& symbol);

    Value string_to_number(const std::string& s);

    void set_error(Error error);

    Environment* env_;
    State state_;
    Stack stack_;
    Error error_;
    Id root_;
    std::string buffer_;

    std::size_t line_num_;
    std::size_t char_num_;
};

} // namespace stree

#endif
