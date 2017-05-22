#ifndef STREE_PARSER_HPP_
#define STREE_PARSER_HPP_

#include <stack>
#include <stdexcept>
#include <string>
#include <stree/environment.hpp>
#include <stree/tree.hpp>

// TODO: Id tree destruction

namespace stree {

// NOTE: terminal function must be written without parenthesis, e.g.:
// (if-some-condition do-something do-something-else)
// instead of
// (if-some-condition (do-something) (do-something-else))

class ParserError : std::exception {
public:
    ParserError(const std::string& what)
        : what_(what) {}

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
        StateExpectNonterm,
        StateTerm,
        StateNontermHead,
        StateNonterm,
        StateNumber,
        StateError,
        StateDone
    };

    enum Error {
        ErrorOk,
        ErrorInvalidChar,
        ErrorUnexpectedLeftParen,
        ErrorUnexpectedRightParen,
        ErrorUnexpectedNumber,
        ErrorUnexpectedNonNumber,
        ErrorExprNotFound,
        ErrorExprUnexpectedTerm,
        ErrorExprUnexpectedNonTerm,
        ErrorTooManyArguments,
        ErrorNotEnoughArguments,
        ErrorNumberInvalid,
        ErrorNumberOutOfRange,
        ErrorNumberUnexpectedDot
    };

    // TODO: copy, assignment
    Parser(Environment& env);
    ~Parser();

    void consume(const char c);

    Id result() const {
        return root_;
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

    std::string error_message() const;

private:
    struct Frame {
        Frame(Id id) : id(id), child_num(0) {}
        Id id;
        unsigned child_num; // current child number
    };
    typedef std::stack<Frame> Stack;

    void reset();
    void count(char c);

    void space();
    void paren_left();
    void paren_right();
    void numeric(char c);
    void dot();
    void ident(char c);

    void complete_term();
    void complete_nonterm_head();
    void complete_nonterm();
    void complete_number();
    void complete_expr(const Symbol* symbol);

    Value string_to_number(const std::string& s);

    void set_error(Error error);

    Environment& env_;
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
