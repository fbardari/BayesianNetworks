// #include "Menu.hpp"

#include "Elimination.hpp"
#include "Network.hpp"
#include "Test.hpp"
#include <iostream>

int main() {

    // Menu menu;
    // menu.run();
    Network net = Test::exampleNetwork();

    std::cout << Elimination::getMarginalProbability(net, "e", "true") << "\n";
    std::cout << Elimination::getMarginalProbability(net, "e", "false") << "\n";

    std::cout << Elimination::getConditionalProbability(
        net,
        "e", "true",
        {
            {"d", "true"}
        }
    ) << "\n";
    std::cout << Elimination::getConditionalProbability(
        net,
        "e", "false",
        {
            {"d", "true"}
        }
    ) << "\n";

}
