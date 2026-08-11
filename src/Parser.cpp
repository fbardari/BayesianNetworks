#include "Parser.hpp"

#include <stdexcept>
#include <iostream>

void Parser::cleanString() {
    for (char c : ",;()[]{}\"") {
        s.erase(remove(s.begin(), s.end(), c), s.end());
    }
}

Parser::Parser (const std::string& filename, bool log)
    : filename(filename), log(log) {}

Network Parser::importBIF(const std::string& filename, bool log) {
    Parser parser(filename, log);
    return parser.parse();
}

Network Parser::parse() {
    // apri file
    file.open(filename);

    // eccezione se non riesci ad aprire il file
    if (!file.is_open()) throw std::runtime_error("Parser::parse: impossibile aprire il file");

    while (file >> s) {
        if (log) std::cout << "Parser::parse: loop principale, letto token " << s << "\n";

        if (s == "network") parseNetwork();
        else if (s == "variable") parseVariable();
        else if (s == "probability") parseProbability();

    }

}

void Parser::parseNetwork() {
    if (log) std::cout << "Parser::parseNetwork: letto token" << s << "\n";
}

void Parser::parseVariable() {
    if (log) std::cout << "Parser::parseVariable: letto token" << s << "\n";
}

void Parser::parseProbability() {
    if (log) std::cout << "Parser::parseProbability: letto token" << s << "\n";
}