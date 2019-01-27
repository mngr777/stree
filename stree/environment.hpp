#ifndef STREE_EXPR_MAP_HPP_
#define STREE_EXPR_MAP_HPP_

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <stree/macros.hpp>
#include <stree/tree.hpp>

namespace stree {

#ifndef STREE_DATA_PTR_TYPE
# define STREE_DATA_PTR_TYPE void*
#endif

using Arguments = std::vector<Value>;
using DataPtr = STREE_DATA_PTR_TYPE;
using Function = std::function<Value(const Arguments&, DataPtr)>;

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
            SelectFunctionIndex sfid;
        } select;
    } data_;
};


class Environment {
public:
    typedef std::vector<const Symbol*> SymbolList;

    Environment() {}
    Environment(const Environment& other) = delete;
    Environment& operator=(const Environment& other) = delete;
    
    void add_function(const std::string& name, Arity arity, Function function);
    Function function(FunctionIndex fid) const;

    void add_positional(const std::string& name, Position position);

    const Symbol* symbol(const std::string& name) const;
    const Symbol* symbol(const Id& id) const;

    unsigned symbol_num() const;
    const Symbol* symbol(unsigned n) const;

    unsigned symbol_by_arity_num(Arity arity) const;
    const Symbol* symbol_by_arity(Arity arity, unsigned n) const;

    unsigned terminal_num() const;
    const Symbol* terminal(unsigned n) const;

    unsigned nonterminal_num() const;
    const Symbol* nonterminal(unsigned n) const;

    Id make_id(const Symbol* symbol);

    const NodeManager& node_manager() const {
        return node_manager_;
    }

    NodeManager& node_manager() {
        return node_manager_;
    }

private:
    using SymbolMap = std::map<std::string, Symbol>;
    using SymbolListArityMap = std::map<Arity, SymbolList>;

    void add_symbol(Symbol symbol);
    void add_symbol_to_arity_lists(const Symbol* symbol);

    SymbolMap symbol_map_;
    SymbolListArityMap symbol_list_arity_map_;
    SymbolList symbols_;
    SymbolList nonterminals_;
    std::vector<Function> functions_;
    NodeManager node_manager_;
};

} // namespace stree

#endif
