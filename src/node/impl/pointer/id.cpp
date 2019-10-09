#include <cassert>
#include <stree/node/impl/pointer/id.hpp>

namespace stree {

bool Id::operator==(const Id& other) const {
    return index_ == other.index_;
}

bool Id::operator!=(const Id& other) const {
    return !(*this == other);
}

std::size_t Id::hash() const {
    return static_cast<std::size_t>(index_);
}

bool Id::empty() const {
    return index_;
}

Type Id::type() const {
}

Arity Id::arity() const {

}

Index Id::index() const {
    return index_;
}

void Id::reset() {
    index_ = NoIndex;
}

} // namespace stree
