#ifndef STREE_TREE_HPP_
#define STREE_TREE_HPP_

#include <cstdint>
#include <limits>
#include <ostream>

namespace stree {
class Id;
class FunctionId;
}
std::ostream& operator<<(std::ostream& os, const stree::Id& id);
std::ostream& operator<<(std::ostream& os, const stree::FunctionId& fid);

namespace stree {

using TypeId = std::uint8_t;
using Arity  = std::uint8_t;
using Index  = std::uint32_t;
using FunctionIndex = std::uint8_t;

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

const std::uint8_t TypeWidth = 2;
const std::uint8_t ArityWidth = 3;
static_assert(TypeWidth + ArityWidth < sizeof(Index) * 8);
static_assert(ArityWidth < sizeof(FunctionIndex) * 8);
constexpr std::uint8_t IndexWidth = (sizeof(Index) * 8 - TypeWidth - ArityWidth);
constexpr std::uint8_t FunctionIndexWidth = (sizeof(FunctionIndex) * 8 - ArityWidth);

class Id {
private:
    static constexpr Index DataMask = std::numeric_limits<Index>::max();
    static constexpr Index TypeMask = (DataMask >> (ArityWidth + IndexWidth)) << (ArityWidth + IndexWidth);
    static constexpr Index IndexMask = (DataMask << (TypeWidth + ArityWidth)) >> (TypeWidth + ArityWidth);
    static constexpr Index ArityMask = DataMask & ~TypeMask & ~IndexMask;

public:
    static constexpr Index NoIndex = IndexMask;

    Id() : Id(TypeConst, 0, NoIndex) {}

    Id(Type type, Arity arity, Index index);

    bool empty() const {
        return index() == NoIndex;
    }

    Type type() const;
    void set_type(Type type);

    Arity arity() const;
    void set_arity(Arity arity);

    Index index() const;
    void set_index(Index index);

private:
    Index data_;
};


class FunctionId {
private:
    static constexpr FunctionIndex DataMask = std::numeric_limits<FunctionIndex>::max();
    static constexpr FunctionIndex FunctionIndexMask = (DataMask << ArityWidth) >> ArityWidth;
    static constexpr FunctionIndex ArityMask = DataMask & ~FunctionIndexMask;
public:
    static constexpr FunctionIndex NoIndex = FunctionIndexMask;

    FunctionId() : FunctionId(0, NoIndex) {}

    FunctionId(Arity arity, FunctionIndex index);

    bool empty() const {
        return index() == NoIndex;
    }

    Arity arity() const;
    void set_arity(Arity arity);

    Index index() const;
    void set_index(FunctionIndex index);

private:
    FunctionIndex data_;
};

} // namespace stree

#endif
