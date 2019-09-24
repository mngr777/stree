#include <iostream>
#include <string>
#include <stree/stree.hpp>
#include "macros.hpp"

DEFUN_EMPTY(func)

int main() {
    using namespace std;
    using namespace stree;

    Environment env;
    env.add_function("+", 2, &::func);
    env.add_function("-", 2, &::func);
    env.add_function("*", 2, &::func);
    env.add_function("%", 2, &::func);
    env.add_positional("a", 0);
    env.add_positional("b", 1);
    env.add_positional("c", 2);
/*
         (+)0
       /      \
      (*)1     (%)2
     /  \     /   \
   (-)3 (a)4 (b)5 (2.0)6
  /  \
(c)7 (1.0)8
*/
    string answer("(+ (* (- c 1) a) (% b 2))");

    // v1
    Builder b1(env);
    b1.set("+"); // set 0
    b1.down(0);  // select 1
    b1.set("*"); // set 1
    b1.down(0);  // select 3
    b1.set("-"); // set 3
    b1.down(0);  // select 7
    b1.set("c"); // set 7
    b1.up();     // select 3
    b1.down(1);  // select 8
    b1.set(1);   // set 8
    b1.up(2);    // select 1
    b1.down(1);  // select 4
    b1.set("a"); // set 4
    b1.up(2);    // select 0
    b1.down(1);  // select 2
    b1.set("%"); // set 2
    b1.down(0);  // select 5
    b1.set("b"); // set 5
    b1.up();     // select 2
    b1.down(1);  // select 6
    b1.set(2);   // set 6

    if (!b1.is_valid()) {
        cerr << "Built tree is not valid" << endl;
        return -1;
    }

    Tree t1(&env, b1.root());
    cout << "Built tree    : " << t1 << endl;
    cout << "Correct answer: " << answer << endl;
    if (to_string(t1) != answer) {
        cerr << "Invalid answer" << endl;
        return -2;
    }

    return 0;
}
