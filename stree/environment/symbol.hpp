#ifndef STREE_ENVIRONMENT_SYMBOL_HPP_
#define STREE_ENVIRONMENT_SYMBOL_HPP_

#include <functional>
#include <vector>
#include <stree/macros.hpp>
#include <stree/types.hpp>

namespace stree {

using Arguments = std::vector<Value>;
using DataPtr = STREE_DATA_PTR_TYPE;
using Function = std::function<Value(const Arguments&, DataPtr)>;
using SelectFunction = std::function<unsigned(const Arguments&, DataPtr)>;

class Symbol {
public:
    Symbol(std::string name, Type type)
        : name_(name),
          type_(type) {}

    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const;

    bool is_callable() const {
        return type_ == TypeFunction || type_ == TypeSelect;
    }

    bool is_variable() const {
        return !is_callable();
    }

    const std::string& name() const {
        return name_;
    }

    std::string& name() {
        return name_;
    }

    // Common
    Type type() const {
        return type_;
    }
    Arity arity() const;

    // Const
    Value value() const;
    void set_value(Value value);

    // Positional
    Position position() const;
    void set_position(Position position);

    // Function, Select
    void set_arity(Arity arity);

    // Function
    FunctionIndex fid() const;
    void set_fid(FunctionIndex fid);

    // Select
    SelectFunctionIndex sfid() const;
    void set_sfid(SelectFunctionIndex sfid);
    Arity sf_arity() const;
    void set_sf_arity(Arity arity);

private:
    std::string name_;
    Type type_;
    union {
        struct {
            Value value;
        } constant;
        struct {
            Position position;
        } positional;
        struct {
            Arity arity;
            FunctionIndex fid;
        } function;
        struct {
            Arity arity;
            Arity sf_arity;
            SelectFunctionIndex sfid;
        } select;
    } data_;
};

}

#endif
