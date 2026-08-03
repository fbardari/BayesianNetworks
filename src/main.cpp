#include <iostream>

#include "Network.hpp"
#include "Utilities.hpp"


// test: vedo se addVariable() è in grado di capire se un network è ciclico
void testCyclicNetwork() {

    Variable v1 = {
        "nome",
        {"true", "false"},
        {1},
        {{0.5, 0.5}}
    };

    Variable v2 = {
        "nome2",
        {"true", "false"},
        {0}, // network ciclico!
        {{0.4, 0.6}, {0.5, 0.5}}
    };

    Network MyNetwork;

    try
    {
        MyNetwork.addVariable(v1);
        MyNetwork.addVariable(v2);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    // deve restituire errore network ciclico come atteso

}

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