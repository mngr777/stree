#ifndef STREE_NODE_IMPL_PACKED_NODE_HPP_
#define STREE_NODE_IMPL_PACKED_NODE_HPP_

#include <stree/types.hpp>

namespace stree {

// Positional argument node
class PositionalNode {
public:
    PositionalNode(Position position = 0)
        : position_(position) {}

    Position position() const {
        return position_;
    }

    void set_position(Position position) {
        position_ = position;
    }

private:
    Position position_;
};


// Constant value node
class ConstNode {
public:
    ConstNode(Value value = Value())
        : value_(std::move(value)) {}

    const Value& value() const {
        return value_;
    }

    void set_value(Value value) {
        value_ = std::move(value);
    }

private:
    Value value_;
};


// Function node
template<Arity A>
class FunctionNode {
public:
    FunctionNode(FunctionIndex fid = FunctionNoIndex)
        : fid_(fid) {}

    FunctionIndex fid() const {
        return fid_;
    }

    void set_fid(FunctionIndex fid) {
        fid_ = fid;
    }

    const Id& argument(Arity n) const {
        assert(0 <= n && n < A);
        return arguments_[n];
    }

    Id& argument(Arity n) {
        assert(0 <= n && n < A);
        return arguments_[n];
    }

    void set_argument(Arity n, Id arg) {
        assert(0 <=n && n < A);
        arguments_[n] = arg;
    }

private:
    FunctionIndex fid_;
    Id arguments_[A];
};


// Select node
template<Arity A>
class SelectNode {
public:
    SelectNode() : SelectNode(SelectFunctionNoIndex) {}

    SelectNode(SelectFunctionIndex sfid) : sfid_(sfid) {}

    SelectFunctionIndex sfid() const {
        return sfid_;
    }

    void set_sfid(FunctionIndex sfid) {
        sfid_ = sfid;
    }

    const Id& argument(Arity n) const {
        assert(0 <= n && n < A);
        return arguments_[n];
    }

    Id& argument(Arity n) {
        assert(0 <= n && n < A);
        return arguments_[n];
    }

    void set_argument(Arity n, Id arg) {
        assert(0 <=n && n < A);
        arguments_[n] = arg;
    }

private:
    SelectFunctionIndex sfid_;
    Id arguments_[A];
};

} // namespace stree

#endif
