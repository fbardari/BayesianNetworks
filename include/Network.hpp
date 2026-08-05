#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <vector>
#include <unordered_map>
#include <stdexcept>

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

// vettore degli id ordinati secondo l'ordine topologico
std::vector<int> topologicalOrder;

// aggiorna il vettore topologicalOrder
void updateTopologicalOrder();

// funzione ricorsiva utilizzata in getMarginalProbability()
double marginalRecursive(int variableId, std::vector<int>& assignment) const;

public:

/* aggiunge un oggetto del tipo variabile al network,
aggiornando la mappa degli id e le liste di adiacenza */
void addVariable(const Variable& variable);

// *** COSTRUTTORI ***

Network(); // crea rete vuota
Network(const std::vector<Variable>& variables); // crea rete partendo da un vettore di variabili

// *** CALCOLO PROBABILITÀ ***

// configurazione completa (assignment) -> JOINT PROBABILITY
double getJointProbability(const std::vector<int>& assignment) const;

// (variabile, assignment COMPLETO) -> riga corrispondente tabella CPT
int getCptRow(const Variable& variable, const std::vector<int>& assignment) const;

// (ID variabile, assignment COMPLETO) -> riga corrispondente tabella CPT
int getCptRow(int variableId, const std::vector<int>& assignment) const;

// (nome variabile, nome del valore assegnato) -> MARGINAL PROBABILITY
double getMarginalProbability(const std::string& variableName, const std::string& valueName) const;

// *** GETTER VARI ***

// restituisce il numero di variabili del network
inline int size() const {
    return variables.size();
}

// getter per topologicalOrder
inline const std::vector<int>& getTopologicalOrder() const {
    return topologicalOrder;
}

// nome variabile -> nomi valori possibili
inline const std::vector<std::string>& getValues(const std::string& variableName) const {
    return variables[getVariableId(variableName)].values;
}

// getter per liste di adiacenza
const std::vector<int>& getChildren(int id) const;

// (ID variabile, nome valore) -> ID valore
int getValueIndex(int variableId, const std::string& valueName) const;

// nome variabile -> ID variabile
int getVariableId(const std::string& name) const;

/*  overload dell'operatore []
network[ID] restituisce una reference alla variabile con quell'ID  */ 
inline const Variable& operator[](int variableId) const {
    if (variableId >= 0 && variableId < variables.size()) {
        return variables[variableId];
    } else {
        throw std::out_of_range("Network::operator[]: non c'è nessuna variabile con id = " + std::to_string(variableId));
    }
}

};

#endif