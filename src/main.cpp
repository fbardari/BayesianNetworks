#include <iostream>

#include "Network.hpp"
#include "Utilities.hpp"


int main() {
    Network network;

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
        {0},
        {{0.4, 0.6}, {0.5, 0.5}}
    };

    network.addVariable(v1);
    network.addVariable(v2);
    network.addVariable(v3);

    std::cout << network.getTopologicalOrder();
}