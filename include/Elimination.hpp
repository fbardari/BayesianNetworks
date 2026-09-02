#ifndef ELIMINATION_HPP
#define ELIMINATION_HPP

#include "Network.hpp"
#include "Factor.hpp"
#include <map>
#include <vector>
#include <string>

class Elimination {
private:
// riferimento costante al network
const Network& net;

/*
data una variabile X del network
costruisce il fattore associato
leggendo nella CPT i valori p(X | ...)
*/
Factor toFactor(int variableId) const;

/* vettore che contiene i fattori costruiti leggendo la CPT
per tutte le variabili della rete */
std::vector<Factor> factors;

// calcola ordine di eliminazione con min-degree
std::vector<int> minDegreeOrder(
    const std::vector<Factor>& initialFactors,
    const std::unordered_set<int>& toEliminate
) const;

public:
// costruttore
Elimination(const Network& network);

/* aggiorna il vettore factors
(da usare ad esempio se la rete viene modificata) */
void updateFactors();

// calcolo probabilità marginali e condizionali
Factor query(
    int targetId, 
    const std::map<int, int>& evidence, 
    const std::vector<int>& customOrder = {}
) const;

/* overload di query() per calcolare
probabilità usando le stringhe
*/
Factor query(
    const std::string& targetName, 
    const std::map<std::string, std::string>& evidenceNames, 
    const std::vector<std::string>& customOrderNames = {}
) const;


/* FUNZIONI WRAPPER per probabilità marginale e condizionale
creando automaticamente istanza di Elimination e runnando la query opportuna */ 

static double getMarginalProbability(
    const Network& inputNetwork,
    const std::string& variableName,
    const std::string& valueName
);

static double getConditionalProbability(
    const Network& inputNetwork,
    const std::string& variableName,
    const std::string& valueName,
    const std::map<std::string, std::string>& evidenceNames
);

};

#endif