#ifndef STREE_ENVIRONMENT_HPP_
#define STREE_ENVIRONMENT_HPP_

#include <string>
#include <vector>
#include <stree/environment/symbol.hpp>
#include <stree/environment/symbol_table.hpp>
#include <stree/macros.hpp>
#include <stree/node.hpp>
#include <stree/types.hpp>

namespace stree {

class Environment {
public:
    Environment()
        : symbol_table_(this) {}
    Environment(const Environment& other) = delete;
    Environment& operator=(const Environment& other) = delete;
    
    void add_function(
        const std::string& name,
        Arity arity,
        Function function,
        Cost cost = 0);
    Function function(FunctionIndex fid) const;

    void add_select_function(
        const std::string& name,
        Arity arity,
        Arity cond_arity,
        SelectFunction select_function,
        Cost cost = 0);
    SelectFunction select_function(SelectFunctionIndex sfid) const;

    void add_positional(const std::string& name, Position position);

    void add_constant(const std::string& name, const Value& value);
    void add_constant(const std::string& name);

    Id make_id(const std::string& name);
    Id make_id(const SymbolPtr& symbol);
    Id make_id(const Value& value);

    const NodeManager& node_manager() const {
        return node_manager_;
    }

    NodeManager& node_manager() {
        return node_manager_;
    }

    const SymbolTable& symbols() const {
        return symbol_table_;
    }

private:
    void add_symbol(const SymbolPtr& symbol);

    std::vector<Function> functions_;
    std::vector<SelectFunction> select_functions_;

    SymbolTable symbol_table_;
    NodeManager node_manager_;
};

} // namespace stree

#endif
