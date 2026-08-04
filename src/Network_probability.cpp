#include "Network.hpp"

double Network::getJointProbability(const std::vector<int>& assignment) const {

    // controllo che l'assignment abbia la dimensione giusta
    if (assignment.size() != size()) {
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