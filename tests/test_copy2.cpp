#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <stree/stree.hpp>
#include "macros.hpp"

DEFUN_EMPTY(func);

using TreeList = std::vector<stree::Tree>;

static TreeList crossover(stree::Tree tree1, stree::Tree tree2, std::random_device& rd);

class Individual {
public:
    Individual(stree::Tree&& tree)
        : tree_(std::move(tree)) {}

    stree::Tree& tree() {
        return tree_;
    }

    const stree::Tree& tree() const {
        return tree_;
    }

private:
    stree::Tree tree_;
};

using Population = std::vector<Individual>;

int main() {
    using namespace std;
    using namespace stree;

    // Init environment
    Environment env;
    env.add_function("+", 2, &func);
    env.add_function("-", 2, &func);
    env.add_function("*", 2, &func);
    env.add_function("%", 2, &func);
    env.add_positional("a", 0);
    env.add_positional("b", 1);
    env.add_positional("c", 2);

    // Test data
    string ts1("(* (+ (- a b) (* c 1)) (+ (- a c) (* c 0.5)))");
    string ts2("(* (% (+ c b) (* b 1)) (* (+ b c) (% 2 1)))");

    // Parse and make trees
    Parser p1(&env);
    PARSE(p1, ts1);
    Tree t1(&env, p1.result());
    PARSE(p1, ts2);
    Tree t2(&env, p1.result());

    // Make children tree
    Population children;
    unsigned child_num = 1000;
    std::random_device rd;
    while (children.size() < child_num) {
        TreeList offspring = crossover(t1, t2, rd);
        for (stree::Tree& tree : offspring)
            children.emplace_back(std::move(tree));
    }

    // Output children
    for (const Individual& individual : children)
        cout << individual.tree() << endl;

    return 0;
}

TreeList crossover(stree::Tree tree1, stree::Tree tree2, std::random_device& rd) {
    using ProbDist = std::uniform_real_distribution<float>;
    using NodeNumDist = std::uniform_int_distribution<stree::NodeNum>;

    bool use_term1 = (ProbDist{0, 1.0}(rd) < 0.2);
    stree::NodeNum p1 = use_term1
        ? NodeNumDist{0, tree1.describe().term_num - 1}(rd)
        : NodeNumDist{0, tree1.describe().nonterm_num - 1}(rd);

    bool use_term2 = (ProbDist{0, 1.0}(rd) < 0.2);
    stree::NodeNum p2 = use_term2
        ? NodeNumDist{0, tree2.describe().term_num - 1}(rd)
        : NodeNumDist{0, tree2.describe().nonterm_num - 1}(rd);

    stree::Subtree sub1 = tree1.sub(p1, use_term1 ? stree::IsTerminalYes : stree::IsTerminalNo);
    stree::Subtree sub2 = tree2.sub(p2, use_term2 ? stree::IsTerminalYes : stree::IsTerminalNo);

    sub1.swap(sub2);

    return TreeList({std::move(tree1), std::move(tree2)});
}
