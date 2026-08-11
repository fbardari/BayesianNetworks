#include <iostream>

#include "Network.hpp"
#include "Utilities.hpp"
#include "Test.hpp"
#include "Parser.hpp"


int main() {
    Network network = Test::exampleNetwork();

    std::cout << network[network.getVariableId("a")].CPT << "\n\n";
    std::cout << network[network.getVariableId("b")].CPT << "\n\n";
    std::cout << network[network.getVariableId("c")].CPT << "\n\n";
    std::cout << network[network.getVariableId("d")].CPT << "\n\n";
    std::cout << network[network.getVariableId("e")].CPT << "\n\n";


    std::cout << Test::normalized(network);


    Network importedNetwork = Parser::importBIF("./data/gradient.bif", true);

    std::cout << importedNetwork.getVariableId("a") << "\n";
    std::cout << network.getValues("b");
}