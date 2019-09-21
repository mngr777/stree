#include <stree/string.hpp>
#include <sstream>

std::ostream& operator<<(std::ostream& os, const stree::Tree& tree) {
    return stree::subtree_to_stream(os, tree.root(), *(tree.env()));
}

std::ostream& operator<<(std::ostream& os, const stree::Subtree& subtree) {
    return stree::subtree_to_stream(os, subtree.root(), *(subtree.env()));
}

namespace stree {

std::string to_string(const Id& id) {
    std::stringstream ss;
    ss << id;
    return ss.str();
}

std::string to_string(const stree::Tree& tree) {
    return subtree_to_string(tree.root(), *(tree.env()));
}

std::string to_string(const stree::Subtree& subtree) {
    return subtree_to_string(subtree.root(), *(subtree.env()));
}

std::string subtree_to_string(const Id& id, const Environment& env) {
    std::stringstream ss;
    subtree_to_stream(ss, id, env);
    return ss.str();
}

} // namespace stree
