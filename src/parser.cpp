#include <stree/parser.hpp>
#include <cassert>
#include <cstring>
#include <memory>
#include <utility>

// NOTE: Digits are not included in identifier simbols
// since they are handled separately (and cannot be a first symbol)
// TODO: separate also sign (+/-) symbols
#define ALPHA  "abcdefghijklmnopqrstuvwxyz"
#define ARITHM "-+*/%"
#define LOGIC  "=<>!&|^"
static char Ident[]  = ALPHA ARITHM LOGIC "_:?@#$";
static char Digits[] = "0123456789";
static char Space[]  = " \t\r\n";

static bool is_space(const char c);
static bool is_paren_left(const char c);
static bool is_paren_right(const char c);
static bool is_numeric(const char c);
static bool is_ident(const char c);
static bool is_dot(const char c);

namespace stree {

Parser::Parser(Environment* env) : env_(env)
{
    assert(env_);
    reset();
}

Parser::~Parser() {
    if (!root_.empty())
        id::destroy_subtree(env_->node_manager(), root_);

    while (!stack_.empty()) {
        Frame& top = stack_.top();
        for (Arity n = 0; n < stack_.top().child_num; ++n) {
            id::destroy_subtree(
                env_->node_manager(),
                id::nth_argument(env_->node_manager(), top.id, n));
        }
        stack_.pop();
    }
}

void Parser::consume(const char c) {
    if (state_ == StateError) {
        return;
    } else if (state_ == StateDone) {
        reset();
    }

    // Count character
    count(c);

    // Process character
    if (is_space(c)) {
        space();
    } else if (is_paren_left(c)) {
        paren_left();
    } else if (is_paren_right(c)) {
        paren_right();
    } else if (is_numeric(c)) {
        numeric(c);
    } else if (is_dot(c)) {
        dot();
    } else if (is_ident(c)) {
        ident(c);
    } else {
        set_error(ErrorInvalidChar);
    }
}

void Parser::reset() {
    // result_ = Tree();
    buffer_.clear();
    while (!stack_.empty())
        stack_.pop();
    state_ = StateReady;
    error_ = ErrorOk;
    line_num_ = 0;
    char_num_ = 0;
}

std::string Parser::state_string() const {
    switch (state_) {
        case StateReady:
            return "Ready";
        case StateExpectCallableSymbol:
            return "Expecting callable symbol";
        case StateVariableSymbol:
            return "Reading variable symbol";
        case StateCallableSymbol:
            return "Reading callable symbol";
        case StateCallableArguments:
            return "Reading callable arguments";
        case StateNumber:
            return "Reading number";
        case StateError:
            return "Error";
        case StateDone:
            return "Done";
        default:
            assert(false && "Undefined state");
    }
}

std::string Parser::error_message() const {
    switch (error_) {
        case ErrorOk:
            return "Ok";
        case ErrorInvalidChar:
            return "Invalid character";
        case ErrorUnexpectedLeftParen:
            return "Unexpected `('";
        case ErrorUnexpectedRightParen:
            return "Unexpected `)'";
        case ErrorUnexpectedNumber:
            return "Unexpected number";
        case ErrorUnexpectedNonNumber:
            return "Unexpected non-numeric character";
        case ErrorSymbolNotFound:
            return "Symbol not found";
        case ErrorSymbolUnexpectedVariable:
            return "Unexpected variable";
        case ErrorSymbolUnexpectedCallable:
            return "Unexpected callable";
        case ErrorTooManyArguments:
            return "Too many arguments";
        case ErrorNotEnoughArguments:
            return "Not enough arguments";
        case ErrorNumberInvalid:
            return "Invalid number";
        case ErrorNumberOutOfRange:
            return "Number is out of range";
        case ErrorNumberUnexpectedDot:
            return "Unexpected dot";
        default:
            assert(false && "Undefined error");
    }
}

void Parser::count(char c) {
    if (c == '\n') {
        ++line_num_;
        char_num_ = 0;
    } else {
        ++char_num_;
    }
}

void Parser::space() {
    switch (state_) {
        case StateVariableSymbol:
            complete_variable();
            break;

        case StateCallableSymbol:
            complete_callable_symbol();
            break;

        case StateNumber:
            complete_number();
            break;

        case StateReady:
        case StateExpectCallableSymbol:
        case StateCallableArguments:
        case StateError:
        case StateDone:
            // do nothing
            break;
    }
}

void Parser::paren_left() {
    switch (state_) {
        case StateReady:
        case StateCallableArguments:
            state_ = StateExpectCallableSymbol;
            break;

        case StateExpectCallableSymbol:
        case StateVariableSymbol:
        case StateCallableSymbol:
        case StateNumber:
            set_error(ErrorUnexpectedLeftParen);
            break;

        case StateError:
        case StateDone:
            // do nothing
            break;
    }
}

void Parser::paren_right() {
    switch (state_) {
        case StateReady:
        case StateExpectCallableSymbol:
            set_error(ErrorUnexpectedRightParen);
            break;

        case StateVariableSymbol:
            complete_variable();
            if (state_ == StateCallableArguments)
                complete_callable();
            break;

        case StateCallableSymbol:
            complete_callable_symbol();
            complete_callable();
            break;

        case StateCallableArguments:
            complete_callable();
            break;

        case StateNumber:
            complete_number();
            if (state_ == StateCallableArguments)
                complete_callable();
            break;

        case StateError:
        case StateDone:
            // do nothing
            break;
    }
}

void Parser::numeric(const char c) {
    switch (state_) {
        case StateReady:
            assert(buffer_.size() == 0);
            buffer_.push_back(c);
            state_ = StateNumber;
            break;

        case StateExpectCallableSymbol:
            set_error(ErrorUnexpectedNumber);
            break;

        case StateVariableSymbol:
        case StateCallableArguments:
        case StateCallableSymbol:
        case StateNumber:
            buffer_.push_back(c);
            break;

        case StateError:
        case StateDone:
            // do nothing
            break;
    }
}

void Parser::dot() {
    switch (state_) {
        case StateReady:
        case StateExpectCallableSymbol:
        case StateVariableSymbol:
        case StateCallableSymbol:
        case StateCallableArguments:
            set_error(ErrorInvalidChar);
            break;

        case StateNumber:
            if (buffer_.find('.') == std::string::npos) {
                numeric('.');
            } else {
                set_error(ErrorNumberUnexpectedDot);
            }

        case StateError:
        case StateDone:
            // do nothing
            break;
    }
}

void Parser::ident(const char c) {
    switch (state_) {
        case StateReady:
            assert(buffer_.size() == 0);
            buffer_.push_back(c);
            state_ = StateVariableSymbol;
            break;

        case StateExpectCallableSymbol:
            assert(buffer_.size() == 0);
            buffer_.push_back(c);
            state_ = StateCallableSymbol;
            break;

        case StateCallableArguments:
            state_ = StateVariableSymbol;
            // fallthru
        case StateVariableSymbol:
        case StateCallableSymbol:
            buffer_.push_back(c);
            break;

        case StateNumber:
            set_error(ErrorUnexpectedNonNumber);
            break;

        case StateError:
        case StateDone:
            // do nothing
            break;
    }
}

void Parser::complete_variable() {
    auto symbol = env_->symbol(buffer_);
    buffer_.clear();
    if (!symbol) {
        set_error(ErrorSymbolNotFound);
    } else if (symbol->is_callable()) {
        set_error(ErrorSymbolUnexpectedCallable);
    } else {
        complete_symbol(symbol);
    }
}

void Parser::complete_callable_symbol() {
    auto symbol = env_->symbol(buffer_);
    buffer_.clear();
    if (!symbol) {
        set_error(ErrorSymbolNotFound);
    } else if (!symbol->is_callable()) {
        set_error(ErrorSymbolUnexpectedVariable);
    } else {
        complete_symbol(symbol);
    }
}

void Parser::complete_callable() {
    assert(
        !stack_.empty()
        && "Non-terminal expression end reached, but stack is empty");
    Frame& top = stack_.top();
    if (top.child_num == top.id.arity()) {
        Id id = top.id;
        stack_.pop();
        if (stack_.empty()) {
            // Nothing left on stack
            root_ = id;
            state_ = StateDone;
        } else {
            Frame& top = stack_.top();
            if (top.child_num < top.id.arity()) {
                id::set_nth_argument(
                    env_->node_manager(),
                    top.id, top.child_num++, id);
            } else {
                id::destroy(env_->node_manager(), id);
                set_error(ErrorTooManyArguments);
            }
        }
    } else {
        set_error(ErrorNotEnoughArguments);
    }
}

void Parser::complete_number() {
    Value number{};
    try {
        number = string_to_number(buffer_);
    } catch (const std::invalid_argument&) {
        set_error(ErrorNumberInvalid);
    } catch (const std::out_of_range&) {
        set_error(ErrorNumberOutOfRange);
    }
    buffer_.clear();

    Symbol symbol("", TypeConst);
    symbol.set_value(number);
    complete_symbol(&symbol);
}


void Parser::complete_symbol(const Symbol* symbol) {
    Id id = env_->make_id(symbol);

    if (stack_.empty()) {
        // Empty stack
        if (id.type() == TypeConst
            && id.type() == TypePositional)
        {
            // Variable: done
            root_ = id;
            state_ = StateDone;
        } else {
            // Callable: push on stack to complete
            stack_.emplace(id);
            state_ = StateCallableArguments;
        }
    } else {
        // There's a tree on stack to complete
        Frame& top = stack_.top();
        if (top.child_num < top.id.arity()) {
            // Add a child to a tree on the top
            if (!symbol->is_callable()) {
                id::set_nth_argument(
                    env_->node_manager(),
                    top.id, top.child_num++, id);
            } else {
                stack_.emplace(id);
            }
            state_ = StateCallableArguments;
        } else {
            id::destroy(env_->node_manager(), id);
            set_error(ErrorTooManyArguments);
        }
    }
}

Value Parser::string_to_number(const std::string& s) {
    return (s.find('.') == std::string::npos)
        ? static_cast<Value>(std::stoll(s))
        : static_cast<Value>(std::stod(s));

}

void Parser::set_error(Error error) {
    state_ = StateError;
    error_ = error;
}

} // namespace stree


bool is_space(const char c) {
    return std::strchr(Space, c) != nullptr;
}

bool is_paren_left(const char c) {
    return c == '(';
}

bool is_paren_right(const char c) {
    return c == ')';
}

bool is_numeric(const char c) {
    return std::strchr(Digits, c) != nullptr;
}

bool is_ident(const char c) {
    return std::strchr(Ident, c) != nullptr;
}

bool is_dot(const char c) {
    return c == '.';
}
