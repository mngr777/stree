#include <cstdint>
#include <iostream>
#include <stree/tree.hpp>

int main() {
    using namespace std;
    using namespace stree;
    using FN0 = FunctionNode<0>;
    using FN2 = FunctionNode<2>;

    NodeManager nm1;
    Index i1 = nm1.alloc<FN2>();
    Index i2 = nm1.alloc<FN0>();
    Index i3 = nm1.alloc<FN0>();


    return 0;
}
