#include "Elimination.hpp"
#include "Network.hpp"
#include "Test.hpp"

#include <iostream>

int main() {
    Network net = Test::exampleNetwork();

    Elimination ve(net);

    // Esegui la query usando direttamente le stringhe
    // Factor res = ve.query("e", { {"a", "false"} });
    Factor res = ve.query("e", {});

    // Leggi i risultati
    const auto& values = net.getValues("e");
    for (size_t i = 0; i < res.table.size(); ++i) {
        std::cout << "P(e = " << values[i] << ") = " << res.table[i] << "\n";
    }
}
