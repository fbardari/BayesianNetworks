#include "Elimination.hpp"

double Elimination::getMarginalProbability(
    const Network& inputNetwork,
    const std::string& variableName,
    const std::string& valueName
) {
    return getConditionalProbability(inputNetwork, variableName, valueName, {});
}

double Elimination::getConditionalProbability(
    const Network& inputNetwork,
    const std::string& variableName,
    const std::string& valueName,
    const std::map<std::string, std::string>& evidenceNames
) {
    Elimination VE(inputNetwork); // crea istanza locale di Elimination

    // elimina variabili
    Factor finalFactor = VE.query(variableName, evidenceNames); // ordine eliminazione default!

    int variableId = inputNetwork.getVariableId(variableName);
    int valueIndex = inputNetwork.getValueIndex(variableId, valueName);

    double result = finalFactor.table[valueIndex];

    return result;
}