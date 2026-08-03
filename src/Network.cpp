#include "Network.hpp"

void Network::addVariable(const Variable& variable) {
    // eccezione se esiste già una variabile con quel nome
    if (id.find(variable.name) != id.end()) {
        throw std::invalid_argument("Esiste già una variabile con nome: " + variable.name);
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

    // TODO: aggiornare ordine topologico quando sarà implementato
}