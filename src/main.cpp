// #include "Menu.hpp"

#include "Elimination.hpp"
#include "Network.hpp"
#include "Test.hpp"
#include <iostream>

int main() {

    // Menu menu;
    // menu.run();
    Network net = Test::exampleNetwork();

    Elimination ve(net);

    Factor res = ve.query("e", {});

    // Leggi i risultati
    const auto& values = net.getValues("e");
    for (size_t i = 0; i < res.table.size(); ++i) {
        std::cout << "P(e = " << values[i] << ") = " << res.table[i] << "\n";
    }
}
