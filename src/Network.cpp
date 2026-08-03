#include "Network.hpp"

#include <stdexcept>
#include <queue>

void Network::addVariable(const Variable& variable) {
    // eccezione se esiste già una variabile con quel nome
    if (id.find(variable.name) != id.end()) {
        throw std::invalid_argument("Esiste già una variabile con nome: " + variable.name);
    }

    // eccezione se un genitore indicato non esiste nel network
    for (int parentId : variable.parents) {
        if (parentId < 0 || parentId >= variables.size()) {
            throw std::invalid_argument(
                "Network::addVariable: genitore con id " + std::to_string(parentId) +
                " non esiste (variabile \"" + variable.name + "\")"
            );
        }
    }

    // trovo id disponibile per nuova variabile
    int variableId = variables.size();

    id[variable.name] = variableId; // aggiungo l'id alla mappa
    variables.push_back(variable); // aggiungo variabile al vettore variables

    // creo spazio in adj per i figli della nuova variabile
    adj.push_back(std::vector<int>());

    /* per ogni genitore 
    aggiungo questa variabile come figlio (in adj) */
    for (int parentId : variable.parents) {
        adj[parentId].push_back(variableId);
    }

    // aggiornare ordine topologico
    updateTopologicalOrder();
}

void Network::updateTopologicalOrder() {
    int n = variables.size(); // quante variabili ci sono nel network

    std::vector<int> parentsCount(n, 0); // numero genitori per ogni variabile
    std::vector<int> result;
    std::queue<int> q; // coda

    for (int i = 0; i < n; i++) {
        parentsCount[i] = variables[i].parents.size(); // conto i genitori di ogni variabile
        if (parentsCount[i] == 0) q.push(i); // se non ha genitori allora metto in coda
    }

    while (!q.empty()) {
        int u = q.front(); // estraggo un nodo senza genitori da processare
        q.pop();
        result.push_back(u); // inserisco in result

        // ciclo sui figli del genitore estratto
        for (int childId : adj[u]) {
            parentsCount[childId]--; // abbiamo processato il genitore

            /* se il figlio non ha più genitori da processare
               metto il figlio in coda */ 
            if (parentsCount[childId] == 0) q.push(childId);
        }
    }

    if (result.size() != n) throw std::runtime_error("Network::updateTopologicalOrder: errore network ciclico");

    topologicalOrder = result;
}