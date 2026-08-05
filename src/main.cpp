#include <iostream>

#include "Network.hpp"
#include "Utilities.hpp"
#include "Test.hpp"


int main() {
    Network network = Test::exampleNetwork();

    std::cout << network[network.getVariableId("a")].CPT << "\n\n";
    std::cout << network[network.getVariableId("b")].CPT << "\n\n";
    std::cout << network[network.getVariableId("c")].CPT << "\n\n";
    std::cout << network[network.getVariableId("d")].CPT << "\n\n";
    std::cout << network[network.getVariableId("e")].CPT << "\n\n";


    std::cout << network.getMarginalProbability("e", "false") << "\n";
    std::cout << network.getMarginalProbability("e", "true") << "\n";


    std::cout << Test::normalized(network);
}