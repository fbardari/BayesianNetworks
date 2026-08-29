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

public:
// costruttore
Elimination(const Network& network);

// calcolo probabilità marginali e condizionali
Factor query(int targetId, 
                const std::map<int, int>& evidence, 
                const std::vector<int>& customOrder = {}) const;

/* overload di query() per calcolare
probabilità usando le stringhe
*/
Factor query(const std::string& targetName, 
                const std::map<std::string, std::string>& evidenceNames, 
                const std::vector<std::string>& customOrderNames = {}) const;
};

#endif