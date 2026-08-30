#include "Elimination.hpp"
#include <algorithm>

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

Factor Elimination::query(
    int targetId, 
    const std::map<int, int>& evidence, 
    const std::vector<int>& customOrder
) const {
    /* creo una copia locale del vettore factors
    su cui verrà effettuata l'eliminazione
    (in base alla specifica query in input) */
    std::vector<Factor> activeFactors = factors;

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

    // ordine di eliminazione?
    std::vector<int> eliminationOrder = customOrder;
    if (eliminationOrder.empty()) { // se non lo passo in input
        for (int i = 0; i < net.size(); ++i) {
            if (i != targetId && !evidence.count(i)) {
                eliminationOrder.push_back(i);
            }
        }
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