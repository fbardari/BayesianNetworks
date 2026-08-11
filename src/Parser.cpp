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
        cleanString();

        if (log) std::cout << "Parser::parse: loop principale, letto token \'" << s << "\'\n";

        // avvia il ciclo corrispondende ad ognuno dei tre tipi di blocchi
        if (s == "network") parseNetwork();
        else if (s == "variable") parseVariable();
        else if (s == "probability") parseProbability();

    }

    Network result(variables);

    if(log) std::cout << "Parser::parse: loop principale, lettura file completata\n";
    return result;
}

void Parser::parseNetwork() {
    while (file >> s) {
        cleanString();
        if (s.empty()) break;
        if (log) std::cout << "Parser::parseNetwork: letto token \'" << s << "\'\n";
    }
}

void Parser::parseVariable() {
    file >> s; // leggo nome variabile
    cleanString();

    std::string variableName = s; // salvo nome variabile
    int variableId = variables.size(); // assegno ID disponibile
    id[variableName] = variableId;

    // creo nuovo oggetto di tipo Variable
    Variable newVariable = {
        variableName, // nome nuova variabile
        {}, // values
        {}, // parents id
        {} // cpt vuota, verrà riempita dopo da parseProbability()...
    };

    if (log) std::cout << "Parser::parseVariable: trovata variabile \'" << variableName << "\', assegnato ID=" << variableId << "\n";

    while(file >> s && s != "{");
    while(file >> s && s != "{"); // aspetto la seconda "{" per la lista variabili

    std::string value;
    while (file >> s && s != "};") {
        cleanString();
        value = s;
        if (log) std::cout << "Parser::parseVariable: trovato valore \'" << value << "\'\n";

        newVariable.values.push_back(value);
    }

    // aggiungo nuova variabile a variables
    variables.push_back(newVariable);
}

void Parser::parseProbability() {
    while (file >> s) { // vado avanti finché non trovo il nome della variabile
        cleanString();
        if (s != "") break;
    }

    std::string childName = s; // salvo nome variabile figlio
    int childId = id[childName]; // recupero il suo ID

    if (log) std::cout << "Parser::parseProbability: trovata variabile figlio \'" << childName << "\'\n";
    

    while (file >> s) {
        /*
        vado avanti finché non capisco se ci sono genitori o no
        se ci sono -> salvo i loro ID in variables[].parents
        */

        if (s == "|") { // ci sono genitori
            if (log) std::cout << ", ha genitori:";

            while(file >> s && s != "{" && s!= ")") { // ciclo su tutti i genitori
                cleanString();
                if (s == "") continue;
                
                // salvo nome genitore
                std::string parentName = s;
                int parentId = id[parentName];

                // log nome genitore
                if (log) std::cout << " " << parentName;

                // salvo id genitore in variables[].parents
                variables[childId].parents.push_back(parentId);
            }

            if (log) std::cout << std::endl;
            break; // vado avanti a leggere la CPT
        } else { // no genitori
            if (log) std::cout << ", non ha genitori\n";
            break; // vado avanti a leggere la CPT
        }
    }

    // ora leggo tabella CPT

    // TODO: IMPLEMENTARE
}