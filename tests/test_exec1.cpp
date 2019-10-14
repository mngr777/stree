#include <iostream>
#include <string>
#include <stree/stree.hpp>
#include "macros.hpp"

using namespace std;
using namespace stree;

DEFUN_PASS(step);
DEFUN_PROGN(progn);

int main() {

    // Init environment
    Environment env;
    env.add_function("progn3", 3, &::progn);
    env.add_function("step", 1, &::step);
    env.add_function("step1", 1, &::step);
    env.add_function("step2", 1, &::step);
    env.add_function("step3", 1, &::step);

    // Test data
    string ts1("(step1 (step2 (step3 0))))");
    string ts2("(progn3 (step 1) (step 2) (step 3))");

    // Parse
    Parser p1(&env);
    // Tree 1
    PARSE(p1, ts1);
    Tree t1(&env, p1.move_result());
    // Tree 2
    PARSE(p1, ts2);
    Tree t2(&env, p1.move_result());

    // Exec
    Params params;

    // Tree 1
    cout << t1 << endl;
    Exec exec1(t1);
    ExecDebug debug1(exec1);
    exec1.init(&params);
    while (!exec1.is_finished()) {
        cout << "Stepping. Trace: " << endl;
        debug1.print_backtrace(cout);
        cout << endl;
        exec1.step();
    }

    cout << endl;

    // Tree 2
    cout << t2 << endl;
    Exec exec2(t2);
    ExecDebug debug2(exec2);
    exec1.init(&params);
    while (!exec1.is_finished()) {
        cout << "Stepping. Trace: " << endl;
        debug1.print_backtrace(cout);
        cout << endl;
        exec1.step();
    }

    return 0;
}
