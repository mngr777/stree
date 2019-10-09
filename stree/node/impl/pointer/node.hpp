#ifndef STREE_NODE_IMPL_POINTER_NODE_HPP_
#define STREE_NODE_IMPL_POINTER_NODE_HPP_

#include <cassert>
#include <stree/types.hpp>

namespace stree {

// Node base
class NodeBase {
public:
    NodeBase(Type type, Arity arity)
        : type_(type),
          arity_(arity) {}

    virtual ~NodeBase() {}

    Type type() const {
        return type_;
    }

    Arity arity() const {
        return arity_;
    }

private:
    Type type_;
    Arity arity_;
};


// Positional argument node
class PositionalNode : public NodeBase {
public:
    PositionalNode(Position position = 0)
        : NodeBase(TypePositional, 0),
          position_(position) {}

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
class ConstNode : public NodeBase {
public:
    ConstNode(Value value = Value())
        : NodeBase(TypeConst, 0), value_(value) {}

    const Value& value() const {
        return value_;
    }

    void set_value(Value value) {
        value_ = value;
    }

private:
    Value value_;
};


// Non-leaf base
template<Arity A>
class NonLeafBase : public NodeBase {
public:
    NonLeafBase(Type type)
        : NodeBase(type, A) {}

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
    Id arguments_[A];
};


// Function node
template<Arity A>
class FunctionNode : public NonLeafBase<A> {
public:
    FunctionNode(FunctionIndex fid = FunctionNoIndex)
        : NonLeafBase<A>(TypeFunction),
          fid_(fid) {}

    FunctionIndex fid() const {
        return fid_;
    }

    void set_fid(FunctionIndex fid) {
        fid_ = fid;
    }

private:
    FunctionIndex fid_;
};


// Select node
template<Arity A>
class SelectNode : public NonLeafBase<A> {
public:
    SelectNode(SelectFunctionIndex sfid = SelectFunctionNoIndex)
        : NonLeafBase<A>(TypeSelect),
          sfid_(sfid) {}

    SelectFunctionIndex sfid() const {
        return sfid_;
    }

    void set_sfid(SelectFunctionIndex sfid) {
        sfid_ = sfid;
    }

private:
    SelectFunctionIndex sfid_;
};

} // namespace stree

#endif
