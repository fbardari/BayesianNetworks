#include <iostream>

#include "Network.hpp"

int main() {
    // test: vedo se addVariable() è in grado di capire se un network è ciclico

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

    // restituisce errore network ciclico come atteso
}