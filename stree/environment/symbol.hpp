#ifndef STREE_ENVIRONMENT_SYMBOL_HPP_
#define STREE_ENVIRONMENT_SYMBOL_HPP_

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <stree/macros.hpp>
#include <stree/types.hpp>

namespace stree {

class Symbol;
using SymbolPtr = std::shared_ptr<Symbol>;
using SymbolPtrList = std::vector<SymbolPtr>; // NOTE: no const

SymbolPtr make_symbol(std::string name, Type type);

class Symbol {
public:
    Symbol(std::string name, Type type)
        : name_(std::move(name)),
          type_(type),
          cost_(0) {}

    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const;

    bool is_terminal() const {
        return arity() == 0;
    }

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

    Type type() const {
        return type_;
    }

    Cost cost() const {
        return cost_;
    }

    void set_cost(Cost cost) {
        cost_ = cost;
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
    Cost cost_;
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
