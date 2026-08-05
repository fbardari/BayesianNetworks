#include <iostream>

#include "Network.hpp"
#include "Utilities.hpp"


int main() {

    Variable a = {
        "a",
        {"true", "false"},
        {},
        {{0.5, 0.5}}
    };

    Variable b = {
        "b",
        {"true", "false"},
        {0},
        {{0.8, 0.2},
        {0.3, 0.7}}
    };

    Variable c = {
        "c",
        {"true", "false"},
        {0},
        {{0.6, 0.4},
        {0.2, 0.8}}
    };

    Variable d = {
        "d",
        {"true", "false"},
        {1, 2},
        {{0.9, 0.1},
        {0.7, 0.3},
        {0.6, 0.4},
        {0.1, 0.9}}
    };
    
    Variable e = {
        "e",
        {"true", "false"},
        {0, 2, 3},
        {{0.95, 0.05},
        {0.85, 0.15},
        {0.75, 0.25},
        {0.5, 0.5},
        {0.8, 0.2},
        {0.6, 0.4},
        {0.3, 0.7},
        {0.1, 0.9}}
    };

    Network network({a,b,c,d,e});

    std::cout << network[network.getVariableId("a")].CPT << "\n\n";
    std::cout << network[network.getVariableId("b")].CPT << "\n\n";
    std::cout << network[network.getVariableId("c")].CPT << "\n\n";
    std::cout << network[network.getVariableId("d")].CPT << "\n\n";
    std::cout << network[network.getVariableId("e")].CPT << "\n\n";

}