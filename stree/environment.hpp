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

    Position position() const;
    void set_position(Position position);

    Arity arity() const;
    void set_arity(Arity arity);

    FunctionIndex fid() const;
    void set_fid(FunctionIndex fid);

private:
    Type type_;
    union {
        struct {
            Position position;
        } positional;
        struct {
            Arity arity;
            FunctionIndex fid;
        } function;
    } data_;
};


class Environment {
public:
    void add_function(const std::string& name, Arity arity, Function function);
    void add_positional(const std::string& name, Position position);

    Symbol* symbol_by_name(const std::string& name);

private:
    using SymbolMap = std::map<std::string, Symbol>;

    void add_symbol(const std::string& name, Symbol symbol);

    SymbolMap symbol_map_;
    std::vector<Function> functions_;
};

} // namespace stree

#endif
