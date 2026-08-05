#include "Test.hpp"


Network Test::exampleNetwork() {

    Network testNetwork;

    Variable a = {
        "a",
        {"true", "false"},
        {},
        {{0.5, 0.5}}
    };
    testNetwork.addVariable(a);

    Variable b = {
        "b",
        {"true", "false"},
        {0},
        { {0.8, 0.2}, {0.3, 0.7} }
    };
    testNetwork.addVariable(b);

    Variable c = {
        "c",
        {"true", "false"},
        {0},
        { {0.6, 0.4}, {0.2, 0.8} }
    };
    testNetwork.addVariable(c);

    Variable d = {
        "d",
        {"true", "false"},
        {1, 2},
        { {0.9, 0.1}, {0.7, 0.3}, {0.6, 0.4}, {0.1, 0.9} }
    };
    testNetwork.addVariable(d);

    Variable e = {
        "e",
        {"true", "false"},
        {0, 2, 3},
        {
            {0.95, 0.05},
            {0.85, 0.15},
            {0.75, 0.25},
            {0.50, 0.50},
            {0.80, 0.20}, 
            {0.60, 0.40},
            {0.30, 0.70},
            {0.10, 0.90}
        }
    };
    testNetwork.addVariable(e);

    return testNetwork;
}


bool Test::normalized(Network& network) {
    return false; // DA IMPLEMENTARE
}