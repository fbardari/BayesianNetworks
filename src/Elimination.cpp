#include "Elimination.hpp"
#include <algorithm>
#include <unordered_set>

Elimination::Elimination(const Network& network) : net(network) {
    updateFactors();
}

void Elimination::updateFactors() {
    factors.clear();
    factors.reserve(net.size());
    for (int i = 0; i < net.size(); ++i) {
        factors.push_back(toFactor(i)); // crea un fattore per ogni variabile della rete
    }
}

Factor Elimination::toFactor(int variableId) const {
    const Variable& var = net[variableId];
    Factor f;

    // aggiungiamo i genitori della variabile nello scope del fattore
    for (int parentId : var.parents) { // per ogni genitore
        f.scope.push_back(parentId); // aggiungi ID allo scope
        int numParentValues = static_cast<int>(net[parentId].values.size()); // numero valori possibili genitore
        f.numValues.push_back(numParentValues); // aggiungi numero valori possibili
    }
    
    // aggiungiamo anche la variabile stessa allo scope del fattore
    f.scope.push_back(variableId);
    int numVariableValues = static_cast<int>(var.values.size());
    f.numValues.push_back(numVariableValues);

    // riempio table leggendo la CPT
    for (const auto& row : var.CPT) {
        for (double prob : row) {
            f.table.push_back(prob);
        }
    }

    return f;
}

std::vector<int> Elimination::minDegreeOrder(
    const std::vector<Factor>& initialFactors,
    const std::unordered_set<int>& toEliminate
) const {
    /* costruisco grafo di interazione (non orientato)
    adiacenza tra le variabili che compaiono insieme
    nello scope di almeno un fattore */ 
    std::unordered_map<int, std::unordered_set<int>> graph;
    for (const auto& f : initialFactors) {
        for (int a : f.scope)
            for (int b : f.scope)
                if (a != b) graph[a].insert(b);
    }

    std::unordered_set<int> remaining = toEliminate;
    std::vector<int> order;

    while (!remaining.empty()) {
        /* scelgo variabile con numero di vicini minimo

        eliminare un nodo con pochi vicini significa che:
        prodotto dei fattori correlati -> tabella più piccola
        */
        int best = -1, bestDegree = -1;
        for (int v : remaining) {
            int degree = static_cast<int>(graph[v].size());
            if (best == -1 || degree < bestDegree) {
                best = v;
                bestDegree = degree;
            }
        }

        /* fill-in
        quando eliminiamo "best", i suoi vicini finiscono nello scope del nuovo fattore generato da sumOut()
        devono essere quindi connessi tutti tra loro per aggiornare il grafo per i passaggi successivi

        i.e. per dare l'ordine corretto di eliminazione,
        il grafo deve rappresentare la struttura dei fattori ATTIVI in quel momento */
        for (int a : graph[best]) {
            for (int b : graph[best]) {
                if (a != b) graph[a].insert(b);
            }
            graph[a].erase(best);
        }
        graph.erase(best);

        order.push_back(best);
        remaining.erase(best);
    }

    return order;
}

Factor Elimination::query(
    int targetId, 
    const std::map<int, int>& evidence, 
    const std::vector<int>& customOrder
) const {
    /* variabili rilevanti = target + evidenza + loro antenati
    le altre non contribuiscono al risultato e vanno escluse subito, */
    std::unordered_set<int> relevant = net.getAncestors(targetId);
    relevant.insert(targetId);
    for (const auto& [evidenceVariable, evidenceValue] : evidence) {
        std::unordered_set<int> evidenceAncestors = net.getAncestors(evidenceVariable);
        relevant.insert(evidenceAncestors.begin(), evidenceAncestors.end());
        relevant.insert(evidenceVariable);
    }

    /* creo una copia locale (filtrata) del vettore factors
    su cui verrà effettuata l'eliminazione
    (in base alla specifica query in input) */
    std::vector<Factor> activeFactors;
    for (int i = 0; i < static_cast<int>(factors.size()); ++i) {
        if (relevant.count(i)) activeFactors.push_back(factors[i]);
    }

    // fai operazione restrict sulle eventuali evidenze
    // ciclo su tutti i fattori
    for (auto& f : activeFactors) {
        // ciclo su tutte le evidenze
        for (const auto& [evidenceVariable, evidenceValue] : evidence) { 
            if (f.contains(evidenceVariable)) { // se il fattore contiene la variabile
                f = f.restrict(evidenceVariable, evidenceValue); // fai operazione restrict
            }
        }
    }

    // ordine di eliminazione (solo variabili rilevanti, calcolate sopra)
    std::vector<int> eliminationOrder = customOrder;
    if (eliminationOrder.empty()) { // se non lo passo in input
        std::unordered_set<int> toEliminate;
        for (int i : relevant) {
            if (i != targetId && !evidence.count(i)) toEliminate.insert(i);
        }
        eliminationOrder = minDegreeOrder(activeFactors, toEliminate);
    }

    // sumOut() cioè eliminazione variabili vera e propria
    for (int varToEliminate : eliminationOrder) {
        /* verifica che la variabile che stiamo eliminando non sia
        né il target né un'evidenza -> in caso contrario skip */
        if (varToEliminate == targetId || evidence.count(varToEliminate)) continue;

        // i fattori che hanno varToEliminate nello scop
        std::vector<Factor> toCombine;

        // gli altri fattori
        std::vector<Factor> remaining;

        for (const auto& f : activeFactors) {
            if (f.contains(varToEliminate)) {
                toCombine.push_back(f);
            } else {
                remaining.push_back(f);
            }
        }

        /* se nessun fattore contiene la variabile da eliminare
        nel suo scope -> skip */
        if (toCombine.empty()) continue;

        // prodotto dei fattori che contengono la variabile
        Factor combined = toCombine[0];
        for (size_t i = 1; i < toCombine.size(); ++i) {
            combined = combined * toCombine[i];
        }

        // marginalizzazione
        // sumOut() sul fattore ottenuto dal prodotto
        Factor newFactor = combined.sumOut(varToEliminate);
        remaining.push_back(newFactor);

        activeFactors = remaining;
    }

    if (activeFactors.empty()) return Factor();

    // prodotto finale di tutti i fattori attivi
    Factor finalFactor = activeFactors[0];
    for (size_t i = 1; i < activeFactors.size(); ++i) {
        finalFactor = finalFactor * activeFactors[i];
    }

    // normalizzazione
    finalFactor.normalize();

    return finalFactor;
}


Factor Elimination::query(
    const std::string& targetName, 
    const std::map<std::string, std::string>& evidenceNames, 
    const std::vector<std::string>& customOrderNames
) const {

    int targetId = net.getVariableId(targetName);

    std::map<int, int> evidence;
    for (const auto& [varName, valName] : evidenceNames) {
        int vId = net.getVariableId(varName);
        int valId = net.getValueIndex(vId, valName);
        evidence[vId] = valId;
    }

    std::vector<int> customOrder;
    for (const auto& name : customOrderNames) {
        customOrder.push_back(net.getVariableId(name));
    }

    return query(targetId, evidence, customOrder);
}