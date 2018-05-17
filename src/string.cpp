#include <stree/string.hpp>


std::ostream& operator<<(std::ostream& os, const stree::Subtree& tree) {
    return id_to_stream(os, tree.root(), *(tree.env()));
}

namespace stree {


} // namespace stree
