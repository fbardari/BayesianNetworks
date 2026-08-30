#include "Elimination.hpp"

double Elimination::getMarginalProbability(
    const Network& inputNetwork,
    const std::string& variableName,
    const std::string& valueName
) {
    Elimination VE(inputNetwork); // crea istanza locale di Elimination

    int variableId = inputNetwork.getVariableId(variableName);
    
    /* elimina variabili ottenendo fattore finale
    contenente probabilità marginali
    per ogni valore possibile del target */
    Factor finalFactor = VE.query(variableId, {}); // ordine eliminazione default!

    // estrai probabilità associata al valore in input
    int valueIndex = inputNetwork.getValueIndex(variableId, valueName);
    double result = finalFactor.table[valueIndex];

    return result;
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