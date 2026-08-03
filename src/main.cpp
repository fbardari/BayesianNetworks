#include <iostream>

#include "Network.hpp"
#include "Utilities.hpp"


int main() {

    Variable v1 = {
        "a",
        {"true", "false"},
        {},
        {{0.5, 0.5}}
    };

    Variable v2 = {
        "b",
        {"true", "false"},
        {0},
        {{0.5, 0.5}, {0.6, 0.4}}
    };

    Variable v3 = {
        "c",
        {"true", "false"},
        {1},
        {{0.4, 0.6}, {0.5, 0.5}}
    };

    Variable v4 = {
        "d",
        {"true", "false"},
        {0},
        {{0.4, 0.6}, {0.5, 0.5}}
    };

    Network network({v1, v2, v3, v4});

    std::cout << network.getTopologicalOrder() << "\n";
    std::cout << network.getChildren(1) << "\n";
    std::cout << network.getValueIndex(1, "true");
}