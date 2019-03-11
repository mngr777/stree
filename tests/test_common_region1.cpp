#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <stree/stree.hpp>
#include "macros.hpp"

DEFUN_EMPTY(func);

int main() {
    using namespace std;
    using namespace stree;

    // Init environment
    Environment env;
    env.add_function("+", 2, &::func);
    env.add_function("-", 2, &::func);
    env.add_function("*", 2, &::func);
    env.add_function("%", 2, &::func);
    env.add_positional("a", 0);
    env.add_positional("b", 1);
    env.add_positional("c", 2);
    env.add_positional("d", 0);
    env.add_positional("e", 1);
    env.add_positional("f", 2);


// Printing region
#define PRINT_REGION(region)                        \
    for (const CommonRegion::Item& item : region)   \
        cout << item.n << ' ';

// Printing answer
#define PRINT_ANSWER(region)                    \
    for (NodeNum n : region)                    \
        cout << n << ' ';

// Comparing region to answer
#define CHECK_REGION(region, answer)                                    \
    if (!std::equal(                                                    \
            region.begin(), region.end(), answer.begin(),               \
            [](const CommonRegion::Item& item, NodeNum n) {             \
                return item.n == n;                                     \
            }))                                                         \
    {                                                                   \
        cout << "Region does not match. Correct answer:" << endl;       \
        PRINT_ANSWER(answer);                                           \
        cout << endl;                                                   \
        return -2;                                                      \
    }

    // Test data
/*
         (+)0
       /      \
      (*)1     (%)2
     /  \     /   \
   (-)3 (a)4 (b)5 (c)6
  /  \
(d)7 (e)8
*/
     string ts1("(+ (* (- d e) a) (% b c))");

/*
        (+)0
     /       \
   (*)1      (%)2
  /   \     /   \
(b)3  (a)4 (b)5 (c)6

common region: 0, 1, 2, 4, 5, 6
*/

     string ts2("(+ (* b a) (% b c))");
     std::vector<NodeNum> cr2_answer({0, 1, 2, 4, 5, 6});

/*
                (+)0
            /         \
        (*)1          (%)2
      /     \         /  \
   (-)3     (+)4    (a)5 (c)6
  /  \      /  \
(e)7 (d)8 (b)9 (f)10

common region: 0, 1, 2, 3, 5, 6, 7, 8
 */
     string ts3("(+ (* (- e d) (+ b f)) (% a c))");
     std::vector<NodeNum> cr3_answer({0, 1, 2, 3, 5, 6, 7, 8});

     // Parse
     Parser p1(&env);
     // tree 1
     PARSE(p1, ts1);
     Tree t1(&env, p1.result());
     // tree 2
     PARSE(p1, ts2);
     Tree t2(&env, p1.result());
     // tree 3
     PARSE(p1, ts3);
     Tree t3(&env, p1.result());

     // Output trees 1, 2
     cout << t1 << endl;
     cout << t2 << endl;
     // Common region for trees 1, 2
     CommonRegion cr2 = common_region(&env, t1.root(), t2.root(), NodeCompare(&env));
     cout << "common nodes: ";
     PRINT_REGION(cr2);
     cout << endl;
     CHECK_REGION(cr2, cr2_answer);
     cout << endl;

     // Output trees 1, 3
     cout << t1 << endl;
     cout << t3 << endl;
     // Common region for trees 1, 3
     CommonRegion cr3 = common_region(&env, t1.root(), t3.root(), NodeCompare(&env));
     cout << "common nodes: ";
     PRINT_REGION(cr3);
     cout << endl;
     CHECK_REGION(cr3, cr3_answer);
     cout << endl;

     return 0;
}
