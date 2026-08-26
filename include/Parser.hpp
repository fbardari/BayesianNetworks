#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <fstream>
#include <string>
#include <unordered_map>

#include "Network.hpp"

class Parser {

private:

std::string filename; // percorso del file BIF
bool log; // fare o non fare print dei log durante il parsing
std::ifstream file; // file stream

std::unordered_map<std::string, int> id; // mappa nome variabili -> ID assegnato
std::vector<Variable> variables; // variabili che costituiranno il network

// stringa letta nel file
std::string s;

// helper per riconoscere i delimitatori
inline bool isDelimiter(char c) {
    return c == ',' || c == ';' || c == '|' ||
           c == '(' || c == ')' || c == '{' || c == '}' || c == '"';
}

// helper per riconoscere spazio + tab + a capo
inline bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/*
ripulisce s, legge prossimo token, salva in s
-> true se trovato, false a fine file
*/
bool nextToken();


// parsing dei blocchi di un file BIF
void parseNetwork();
void parseVariable();
void parseProbability();


// legge il nome della variabile figlio del blocco probability corrente
// -> restituisce il suo ID
int readProbabilityChild();

// legge (se presenti) i genitori dopo il carattere '|'
// e li salva in variables[childId].parents
void readParents(int childId);

// capisce se la CPT ha genitori o no e chiama la funzione corretta
void readCptTable(int childId);

// legge la CPT nel caso senza genitori
void readCptTableNoParents(int childId);

// legge la CPT riga per riga nel caso con genitori
void readCptRows(int childId);


/*
prende in input (variabile child, tutte le altre variabili, assignment PARZIALE)
-> restituisce riga della tabella CPT in cui devo memorizzare l'assignment
*/
int getCptRow(
    int childId,
    const std::unordered_map<int, int>& partialAssignment
);

public:

Parser(const std::string& filename, bool log); // constructor

Network parse(); // avvia il parsing

// restituisce il Network dato un file BIF
static Network importBIF(const std::string& filename, bool log);

};

#endif