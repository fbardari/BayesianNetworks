#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
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

// overload: stessa cosa ma per assignment PARZIALI sui genitori di una variabile
int getCptRow(
    int variableId, // ID variabile
    const std::vector<int>& assigment, // assignment SOLO GENITORI
    const std::unordered_map<int, int>& localIndexMap // mappa: ID variabile -> posizione in assignment
) const;

// (nome variabile, nome del valore assegnato) -> MARGINAL PROBABILITY
double getMarginalProbability(const std::string& variableName, const std::string& valueName) const;

// *** GETTER VARI ***

// restituisce il numero di variabili del network
inline int size() const {
    return static_cast<int>(variables.size());
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

std::vector<std::string> getNames() const; // getter per lista nomi variabili

// restituisce l'insieme degli antenati di una variabile
std::unordered_set<int> getAncestors(int variableId) const;

// conta numero di archi del grafo
inline int arcsCount() const {
    int result = 0;
    for (const auto& v : variables) result += v.parents.size();
    return result;
}

/*  overload dell'operatore []
network[ID] restituisce una reference alla variabile con quell'ID  */ 
inline const Variable& operator[](int variableId) const {
    if (variableId >= 0 && static_cast<size_t>(variableId) < variables.size()) {
        return variables[variableId];
    } else {
        throw std::out_of_range("Network::operator[]: non c'è nessuna variabile con id = " + std::to_string(variableId));
    }
}

};


/*
alcuni OVERLOAD dell'operatore "<<"
(le definizioni sono nel file "Network_print.cpp")
*/

//   cout << variable; -> restituisce tutte le info della variabile
std::ostream& operator<<(std::ostream& os, const Variable& variable);

/*   cout << network; -> restituisce alcune info del network,
                       incluse liste di adiacenza */
std::ostream& operator<<(std::ostream& os, const Network& network);


#endif