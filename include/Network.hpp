#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <vector>
#include <unordered_map>

#include "Variable.hpp"

class Network {

private:

/* 
vettore che contiene tutte le variabili del network
ordinate secondo il loro ID
*/
std::vector<Variable> variables;

/*
liste di adiacenza
adj[ID] = lista dei figli di una variabile dato il suo ID
*/
std::vector<std::vector<int>> adj;

// mappa: name -> id
std::unordered_map<std::string, int> id;

public:

/* aggiunge un oggetto del tipo variabile al network,
aggiornando la mappa degli id e le liste di adiacenza */
void addVariable(const Variable& variable);

};


#endif