#ifndef FACTOR_HPP
#define FACTOR_HPP

#include <vector>

class Factor {
public:

// variabili dello scope del fattore
std::vector<int> scope;

// valori possibili per ogni variabile
std::vector<int> numValues;

// tabella contenente i valori associati ad ogni assignment
std::vector<double> table;

// assignmente completo di variabili dello scope -> indice riga di table
int getTableIndex(const std::vector<int>& assignment) const;

// indice riga di table -> assignment associato
std::vector<int> getAssignment(int flatIndex) const;

// restituisce indice in cui trovare una variabile nello scope
int indexOf(int variableId) const;

// dice se lo scope contiene quella variabile
bool contains(int variableId) const;

// elimina una variabile fissandone il valore
Factor restrict(int variableId, int valueIndex) const;

/*
elimina una variabile
sommando su tutti i suoi possibili valori
*/
Factor sumOut(int variableId) const;

/*
divide tutti i valori di table per la loro somma
cioè trasforma il fattore in una distribuzione di probabilità
*/ 
void normalize();
};

Factor operator*(const Factor& f1, const Factor& f2);

#endif