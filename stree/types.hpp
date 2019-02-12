#ifndef STREE_TYPES_HPP_
#define STREE_TYPES_HPP_

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stree/macros.hpp>

namespace stree {

// Basic types
using TypeId = std::uint8_t;
using Arity  = std::uint8_t;
using Index  = std::uint32_t;
using FunctionIndex       = std::uint8_t;
using SelectFunctionIndex = std::uint8_t;
using Position = std::uint8_t;
using Value    = STREE_VALUE_TYPE;

// "empty" values
const FunctionIndex FunctionNoIndex =
    std::numeric_limits<FunctionIndex>::max();
const SelectFunctionIndex SelectFunctionNoIndex =
    std::numeric_limits<SelectFunctionIndex>::max();


// Type enum and conversions

enum Type : TypeId {
    TypeConst      = 0,
    TypePositional = 1,
    TypeFunction   = 2,
    TypeSelect     = 3
};

inline Type type_id_to_type(TypeId id) {
    return static_cast<Type>(id);
}

inline TypeId type_to_type_id(Type type) {
    return static_cast<TypeId>(type);
}

const char* type_to_string(Type type);


// Tree node number type
using NodeNum = unsigned;
// "empty" tree node number
const NodeNum NoNodeNum = std::numeric_limits<NodeNum>::max();

} // namespace stree

#endif
