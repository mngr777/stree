#include <cassert>
#include <stree/node/impl/pointer/id.hpp>
#include <stree/node/impl/pointer/node.hpp>

namespace stree {

bool Id::operator==(const Id& other) const {
    return index_ == other.index_;
}

bool Id::operator!=(const Id& other) const {
    return !(*this == other);
}

std::size_t Id::hash() const {
    return reinterpret_cast<std::size_t>(index_);
}

bool Id::empty() const {
    return !index_;
}

Type Id::type() const {
    assert(index_);
    return index_->type();
}

Arity Id::arity() const {
    assert(index_);
    return index_->arity();
}

Id::Index Id::index() const {
    return index_;
}

void Id::reset() {
    index_ = NoIndex;
}

} // namespace stree
