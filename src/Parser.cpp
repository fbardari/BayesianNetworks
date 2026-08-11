#include "Parser.hpp"

Parser::Parser (const std::string& filename, bool log)
    : filename(filename), log(log) {}

Network Parser::importBIF(const std::string& filename, bool log) {
    Parser parser(filename, log);
    return parser.parse();
}

void Parser::parseNetwork() {

}

void Parser::parseVariable() {

}

void Parser::parseProbability() {

}