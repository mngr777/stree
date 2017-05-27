#ifndef STREE_EXPR_MAP_HPP_
#define STREE_EXPR_MAP_HPP_

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <stree/macros.hpp>
#include <stree/tree.hpp>

namespace stree {

using Arguments = std::vector<Value>;
using Function = std::function<Value(const Arguments&)>;

class Symbol {
public:
    Symbol(Type type) : type_(type) {}

    bool is_callable() const {
        return type_ == TypeFunction || type_ == TypeSelect;
    }

    bool is_variable() const {
        return !is_callable();
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

private:
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
            SelectFunctionIndex sfid;
        } select;
    } data_;
};


class Environment {
public:
    void add_function(const std::string& name, Arity arity, Function function);
    void add_positional(const std::string& name, Position position);

    const Symbol* symbol(const std::string& name) const;
    const Symbol* symbol(Id id) const;

    Id make_id(const Symbol* symbol);

    NodeManager& node_manager() {
        return node_manager_;
    }

private:
    using SymbolMap = std::map<std::string, Symbol>;

    void add_symbol(const std::string& name, Symbol symbol);

    SymbolMap symbol_map_;
    std::vector<Function> functions_;
    NodeManager node_manager_;
};

} // namespace stree

#endif
