#include <stree/string.hpp>


std::ostream& operator<<(std::ostream& os, const stree::Tree& tree) {
    return id_to_stream(os, tree.root(), *(tree.env()));
}

namespace stree {


} // namespace stree
