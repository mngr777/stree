#include <iostream>
#include <string>
#include <stree/stree.hpp>
#include "macros.hpp"

unsigned select0(const stree::Arguments&, stree::DataPtr) {
    return 0;
}

unsigned select1(const stree::Arguments&, stree::DataPtr) {
    return 1;
}

stree::Value inc(const stree::Arguments&, stree::DataPtr data) {
    *static_cast<unsigned*>(data) += 1;
    return stree::Value{};
}

int main() {
    using namespace std;
    using namespace stree;

    // Init environment
    Environment env;
    env.add_select_function("select0", 2, 0, &::select0);
    env.add_select_function("select1", 2, 0, &::select1);
    env.add_function("inc", 0, &::inc);

    // Test data
/*
    (select0)0
     /     \
  (1.0)1  (inc)2
*/
    string ts1("(select0 1.0 (inc))");
    unsigned data1 = 0;
    Value answer1 = 1.0;
    unsigned data_answer1 = 0;

/*
    (select1)0
     /     \
  (1.0)1  (inc)2
*/
    string ts2("(select1 1.0 (inc))");
    unsigned data2 = 0;
    Value answer2 = Value{};
    unsigned data_answer2 = 1;

    // Parse
    Parser p1(&env);
    // tree 1
    PARSE(p1, ts1);
    Tree t1(&env, p1.result());
    // tree 2
    PARSE(p1, ts2);
    Tree t2(&env, p1.result());

    // Check tree 1
    cout << t1 << endl;
    CHECK_EVAL(t1, {}, answer1, &data1);
    cout << "Data value: " << data1 << endl;
    if (data1 != data_answer1) {
        cout << "Incorrect data value " << data1 << ", expected " << data_answer1;
        return -3;
    }
    cout << endl;

    // Check tree2
    cout << t2 << endl;
    CHECK_EVAL(t2, {}, answer2, &data2);
    cout << "Data value: " << data2 << endl;
    if (data2 != data_answer2) {
        cout << "Incorrect data value " << data2 << ", expected " << data_answer2;
        return -4;
    }

    return 0;
}
