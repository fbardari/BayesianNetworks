#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <fstream>
#include <string>
#include <unordered_map>

#include "Network.hpp"

class Parser {

private:

std::string filename;
bool log;
std::ifstream file;

std::unordered_map<std::string, int> id;
std::vector<Variable> variables;

// stringa letta nel file
std::string s;

/* pulisce la string "s"
    rimuovendo i caratteri presenti nei file BIF
    che non contengono dati */ 
void cleanString();

// parsing dei blocchi di un file BIF
void parseNetwork();
void parseVariable();
void parseProbability();

public:

Parser(const std::string& filename, bool log); // constructor

Network parse(); // avvia il parsing

// restituisce il Network dato un file BIF
static Network importBIF(const std::string& filename, bool log);

};

#endif