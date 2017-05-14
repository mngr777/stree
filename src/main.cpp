#include <cstdint>
#include <iostream>
#include <stree/tree.hpp>

int main() {
    using namespace std;
    using namespace stree;

    Id id1;
    id1.set_type(TypeFunction);
    id1.set_arity(3);
    id1.set_index(93);
    cout << id1 << endl;

    Id id2;
    cout << id2 << endl;

    FunctionId fid1;
    fid1.set_arity(2);
    fid1.set_index(144);
    cout << fid1 << endl;

    FunctionId fid2;
    cout << fid2 << endl;

    return 0;
}
