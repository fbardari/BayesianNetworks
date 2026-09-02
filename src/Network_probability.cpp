#include "Network.hpp"

double Network::getJointProbability(const std::vector<int>& assignment) const {

    // controllo che l'assignment abbia la dimensione giusta
    if (assignment.size() != static_cast<size_t>(this->size())) {
        throw std::invalid_argument("Network::getJointProbability: errore nel calcolo della joint probability, l'assignment ha una dimensione errata");
    }

    double result = 1.0;

    for (int id = 0; id < static_cast<int>(size()); id++) {
        int row = getCptRow(variables[id], assignment);
        int column = assignment[id];

        result *= variables[id].CPT[row][column];
    }

    return result;
}


double Network::getMarginalProbability(const std::string& variableName, const std::string& valueName) const {

    // ricavo indici della variabile e del valore assegnato
    int variableId = getVariableId(variableName);
    int valueIndex = getValueIndex(variableId, valueName);

    // variabili rilevanti = target + suoi antenati
    std::unordered_set<int> relevant = getAncestors(variableId);
    relevant.insert(variableId);

    // ordine topologico filtrato lasciando solo variabili rilevanit
    std::vector<int> relevantOrder;
    relevantOrder.reserve(relevant.size());
    for (int id : topologicalOrder) {
        if (relevant.count(id)) relevantOrder.push_back(id);
    }

    // mappa: ID globale variabile -> posizione locale nel vettore assignment
    std::unordered_map<int, int> localIndex;
    localIndex.reserve(relevantOrder.size());
    for (size_t i = 0; i < relevantOrder.size(); i++) {
        localIndex[relevantOrder[i]] = static_cast<int>(i);
    }

    // stato iniziale (size = numero variabili rilevanti)
    // assignment con solo target fissato, p = 1.0
    std::vector<int> initialAssignment(relevantOrder.size(), -1);
    initialAssignment[localIndex[variableId]] = valueIndex;

    std::vector<std::vector<int>> states;
    std::vector<double> stateProbs;

    states.push_back(initialAssignment);
    stateProbs.push_back(1.0);

    
    // ciclo sulle variabili rilevanti (ordinate topologicamente)
    for (int id : relevantOrder) {

        std::vector<std::vector<int>> nextStates;
        std::vector<double> nextProbs;

        int idx = localIndex[id];

        // scorro tutti gli stati correnti
        for (int i = 0; i < static_cast<int>(states.size()); i++) {

            std::vector<int> assignment = states[i];
            double prob = stateProbs[i];

            if (assignment[idx] != -1) {
                // variabile già fissata (target)
                // moltiplico solo il suo fattore CPT
                int row = getCptRow(id, assignment, localIndex);
                double factor = variables[id].CPT[row][assignment[idx]];

                nextStates.push_back(assignment);
                nextProbs.push_back(prob * factor);

            } else {
                // variabile libera
                // espando per ogni valore possibile
                int possibleValues = static_cast<int>(variables[id].values.size());

                for (int v = 0; v < possibleValues; v++) {
                    std::vector<int> newAssignment = assignment;
                    newAssignment[idx] = v;

                    int row = getCptRow(id, newAssignment, localIndex);
                    double factor = variables[id].CPT[row][v];

                    nextStates.push_back(newAssignment);
                    nextProbs.push_back(prob * factor);
                }
            }
        }

        states = nextStates;
        stateProbs = nextProbs;
    }

    // somma delle probabilità di tutti gli stati finali
    double result = 0.0;
    for (size_t i = 0; i < stateProbs.size(); i++) {
        result += stateProbs[i];
    }

    return result;
}