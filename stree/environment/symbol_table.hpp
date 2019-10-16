#ifndef STREE_ENVIRONMENT_SYMBOL_TABLE_HPP_
#define STREE_ENVIRONMENT_SYMBOL_TABLE_HPP_

#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include <stree/environment/symbol.hpp>
#include <stree/types.hpp>

namespace stree {

class SymbolTable {
public:
    void add(const SymbolPtr& symbol);

    const SymbolPtr& operator[](std::size_t n) const;
    std::size_t size() const;

    const SymbolPtr& by_name(const std::string& name) const;
    const SymbolPtrList& list_by_arity(const Arity arity) const;

    const SymbolPtr& by_position(Position position) const;
    const SymbolPtr& by_fid(FunctionIndex fid) const;
    const SymbolPtr& by_sfid(SelectFunctionIndex sfid) const;

    const SymbolPtrList& terminals() const {
        return terminals_;
    }

    const SymbolPtrList& nonterminals() const {
        return nonterminals_;
    }

private:
    using SymbolPtrMap = std::map<const std::string, SymbolPtr>;
    using SymbolPtrListArityMap = std::map<const Arity, SymbolPtrList>;
    using PositionalMap = std::map<const Position, SymbolPtr>;
    using FunctionMap = std::map<const FunctionIndex, SymbolPtr>;
    using SelectFunctionMap = std::map<const SelectFunctionIndex, SymbolPtr>;

    void add_to_map(const SymbolPtr& symbol);
    void add_to_list(const SymbolPtr& symbol);
    void add_to_term_list(const SymbolPtr& symbol);
    void add_to_arity_list(const SymbolPtr& symbol);
    void add_to_type_map(const SymbolPtr& symbol);

    void add_to_positional_map(const SymbolPtr& symbol);
    void add_to_function_map(const SymbolPtr& symbol);
    void add_to_select_function_map(const SymbolPtr& symbol);

    SymbolPtrList list_;
    SymbolPtrMap map_;
    SymbolPtrList terminals_; // NOTE: redundant (same as list_arity_map_[0])
    SymbolPtrList nonterminals_;
    SymbolPtrListArityMap list_arity_map_;

    PositionalMap positional_map_;
    FunctionMap function_map_;
    SelectFunctionMap select_function_map_;
};

}

#endif
