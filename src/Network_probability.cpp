#include "Network.hpp"

double Network::getJointProbability(const std::vector<int>& assignment) const {

    // controllo che l'assignment abbia la dimensione giusta
    if (assignment.size() != this->size()) {
        throw std::invalid_argument("Network::getJointProbability: errore nel calcolo della joint probability, l'assignment ha una dimensione errata");
    }

    double result = 1.0;

    for (int id : topologicalOrder) {
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

    /* l'assignment è inizializzato tutto a -1
    tranne per la variabile target, il cui valore è fissato */
    std::vector<int> assignment(this->size(), -1);
    assignment[variableId] = valueIndex;

    // start algoritmo ricorsivo
    double result = marginalRecursive(0, assignment);

    return result;
}


double Network::marginalRecursive(int variableId, std::vector<int>& assignment) const {

    if (variableId == this->size()) { // abbiamo assegnato un valore a ogni variabile
        return getJointProbability(assignment); // allora chiama joint probability
    }

    if (assignment[variableId] != -1) {
        return marginalRecursive(variableId+1, assignment);
    }

    double result = 0.0;

    int possibleValues = variables[variableId].values.size();

    for (int v = 0; v < possibleValues; v++) {
        assignment[variableId] = v;
        result += marginalRecursive(variableId+1, assignment);
    }

    assignment[variableId] = -1;
    return result;
}