#include <iostream>

#include "Network.hpp"
#include "Utilities.hpp"
#include "Test.hpp"
#include "Parser.hpp"


int main() {
    Network importedNetwork = Parser::importBIF("./data/asia.bif", false);

    std::cout << Test::normalized(importedNetwork);
}