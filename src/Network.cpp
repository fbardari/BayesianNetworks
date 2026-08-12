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


Network::Network() = default;

Network::Network(const std::vector<Variable>& variables) {
    int n = variables.size();

    this->variables.reserve(n);
    adj.resize(n);

    // salvo variabili e costruisco la mappa id, controllando nomi duplicati
    for (int i = 0; i < n; i++) {
        if (this->id.find(variables[i].name) != this->id.end()) {
            throw std::invalid_argument("Esiste già una variabile con nome: " + variables[i].name);
        }
        this->variables.push_back(variables[i]);
        this->id[variables[i].name] = i;
    }

    // aggiorno adj, controllando che ogni parentId sia valido
    for (int i = 0; i < n; i++) {
        for (int parentId : variables[i].parents) {
            if (parentId < 0 || parentId >= n) {
                throw std::invalid_argument(
                    "Network::Network: genitore con id " + std::to_string(parentId) +
                    " non esiste (variabile \"" + variables[i].name + "\")"
                );
            }
            adj[parentId].push_back(i);
        }
    }

    updateTopologicalOrder(); // aggiorna ordine topologico
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

const std::vector<int>& Network::getChildren(int id) const {
    if (id >= 0 && id < size()) {
        return adj[id];
    } else {
        throw std::out_of_range("Network::getChildren: nessun elemento con id = " + std::to_string(id));
    }
}


int Network::getVariableId(const std::string& name) const {
        auto iterator = id.find(name);
        if (iterator == id.end())
            throw std::invalid_argument("Network::getVariableId: variabile \"" + name + "\" non trovata");
        return iterator->second;
}

int Network::getValueIndex(int variableId, const std::string& valueName) const {
    const Variable& variable = (*this)[variableId];

    for (int i = 0; i < variable.values.size(); i++) {
        if (variable.values[i] == valueName) return i;
    }

    throw std::invalid_argument("Network::getValueIndex: non c'è nessun valore di nome \"" + valueName + "\" per la variabile \"" + variables[variableId].name + "\"");
}

int Network::getCptRow(const Variable& variable, const std::vector<int>& assignment) const {
    int row = 0; // riga
    int mult = 1; // multiplier

    /*  ciclo su tutti i genitori di variable
        parto dall'ultimo (il valore cambia più velocemente) */
    for (int i = variable.parents.size()-1; i>=0; i--) {
        int parentId = variable.parents[i];
        int assignedValueId = assignment[parentId];

        row += assignedValueId * mult;
        mult *= variables[parentId].values.size();
    }

    return row;
}

int Network::getCptRow(int variableId, const std::vector<int>& assignment) const {
    return getCptRow((*this)[variableId], assignment);
}

std::unordered_set<int> Network::getAncestors(int variableId) const {
    std::unordered_set<int> visited;
    std::queue<int> q;
    q.push(variableId);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int parentId : variables[u].parents) {
            if (visited.insert(parentId).second) { // true se non era già presente
                q.push(parentId);
            }
        }
    }

    return visited; // non include variableId stesso
}