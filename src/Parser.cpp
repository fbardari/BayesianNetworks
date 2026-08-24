#include "Parser.hpp"

#include <stdexcept>
#include <iostream>
#include <algorithm>

void Parser::cleanString() {
    for (char c : ",;()[]{}\"") {
        s.erase(std::remove(s.begin(), s.end(), c), s.end());
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
    int variableId = static_cast<int>(variables.size()); // assegno ID disponibile
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
    int childId = readProbabilityChild();
    readParents(childId);

    int numRows = 1;
    for (int parentId : variables[childId].parents) {
        numRows *= variables[parentId].values.size();
    }
    variables[childId].CPT.assign(numRows, std::vector<double>());

    readCptTable(childId);
}

int Parser::readProbabilityChild() {
    while (file >> s) { // vado avanti finché non trovo il nome della variabile
        cleanString();
        if (s != "") break;
    }

    std::string childName = s;
    int childId = id[childName];

    if (log) std::cout << "Parser::parseProbability: trovata variabile figlio \'" << childName << "\'\n";

    return childId;
}

void Parser::readParents(int childId) {
    while (file >> s) {
        if (s == "|") { // ci sono genitori
            if (log) std::cout << ", ha genitori:";

            while (file >> s && s != "{" && s != ")") { // ciclo su tutti i genitori
                cleanString();
                if (s.empty()) continue;

                std::string parentName = s;
                int parentId = id[parentName];

                if (log) std::cout << " " << parentName;

                variables[childId].parents.push_back(parentId);
            }

            if (log) std::cout << std::endl;
            return; // vado avanti a leggere la CPT
        } else { // no genitori
            if (log) std::cout << ", non ha genitori\n";
            return; // vado avanti a leggere la CPT
        }
    }
}

void Parser::readCptTable(int childId) {
    while (file >> s) {
        cleanString();
        if (s.empty()) continue;

        if (s == "table") {
            readCptTableNoParents(childId);
        } else {
            readCptRows(childId);
        }
        return;
    }
}

void Parser::readCptTableNoParents(int childId) {
    if (log) std::cout << "Parser::parseProbability: salvate probabilità";

    while (file >> s && s != "}") {
        cleanString();
        if (s.empty()) continue;

        double prob = std::stod(s);
        variables[childId].CPT[0].push_back(prob);
        if (log) std::cout << " " << prob;
    }

    if (log) std::cout << "\n";
}

void Parser::readCptRows(int childId) {
    int numParents = variables[childId].parents.size();
    int numValues = variables[childId].values.size();

    if (log) std::cout << "Parser::parseProbability: numParents=" << numParents << ", numValues=" << numValues << std::endl;

    int numRows = variables[childId].CPT.size();

    for (int row = 0; row < numRows; row++) {
        std::unordered_map<int, int> partialAssignment;

        if (log) std::cout << "Parser::parseProbability: leggo riga " << row << std::endl;
        if (log) std::cout << "\tleggo valori ";

        // ciclo su ogni valore genitore della riga
        for (int i = 0; i < numParents; i++) {
            cleanString();
            while (s.empty()) {
                file >> s;
                cleanString();
            }

            std::string valueName = s;
            int parentId = variables[childId].parents[i];

            int valueId = -1;
            for (size_t v = 0; v < variables[parentId].values.size(); v++) {
                if (variables[parentId].values[v] == valueName) {
                    valueId = v;
                    break;
                }
            }
            partialAssignment[parentId] = valueId;

            if (log) std::cout << " " << valueName << "(parentId=" << valueId << ",valueId=" << valueId << ")";

            file >> s;
        }
        if (log) std::cout << std::endl;

        int cptRow = getCptRow(childId, partialAssignment);

        if (log) std::cout << "\tleggo probabilita ";

        // ciclo su ogni probabilità della riga
        for (int i = 0; i < numValues; i++) {
            cleanString();
            while (s.empty()) {
                file >> s;
                cleanString();
            }

            double prob = std::stod(s);
            if (log) std::cout << " " << prob;

            variables[childId].CPT[cptRow].push_back(prob);

            file >> s;
        }
        if (log) std::cout << std::endl;
    }
}


int Parser::getCptRow(
    int childId,
    const std::unordered_map<int, int>& partialAssignment
) {
    int row = 0; // riga
    int mult = 1; // multiplier

    /*  ciclo su tutti i genitori della variabile child
        parto dall'ultimo (il valore cambia più velocemente) */
    for (int i = static_cast<int>(variables[childId].parents.size()) - 1; i >= 0; i--) {
        int parentId = variables[childId].parents[i];

        auto iter = partialAssignment.find(parentId);

        if (iter == partialAssignment.end()) throw std::runtime_error("Parser::getCptRow: genitore mancante");

        int assignedValueId = iter->second;
        row += assignedValueId * mult;
        mult *= variables[parentId].values.size();
    }

    return row;
}