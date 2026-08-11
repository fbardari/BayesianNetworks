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

void parseNetwork();
void parseVariable();
void parseProbability();

public:

Parser(const std::string& filename, bool log);

Network parse();

static Network importBIF(const std::string& filename, bool log);

};

#endif