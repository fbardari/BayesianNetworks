#include "Parser.hpp"

#include <stdexcept>
#include <iostream>
#include <algorithm>

bool Parser::nextToken() {
    s.clear(); // pulisco s
    char c;

    // salto spazio
    while (file.get(c)) {
        if (isWhitespace(c)) continue;
        break;
    }

    if (!file) return false; // end of file

    // se è un delimitatore, il token è il singolo carattere
    if (isDelimiter(c)) {
        s = std::string(1, c);
        return true;
    }

    // altrimenti accumulo caratteri finché non trovo spazio o delimitatore
    s += c;
    while (file.get(c)) {
        if (isWhitespace(c) || isDelimiter(c)) {
            file.unget(); // rimetto il carattere nello stream, sarà letto dopo
            break;
        }
        s += c;
    }

    return true;
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

    while (nextToken()) {

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
    while (nextToken()) {
        if (s == "}") break;
        if (log) std::cout << "Parser::parseNetwork: letto token \'" << s << "\'\n";
    }
}

void Parser::parseVariable() {
    nextToken(); // leggo nome variabile
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

    while (nextToken() && s != "type" && s != "}");

    if (s != "type") {
        throw std::invalid_argument("Parser::parseVariable: la variabile " + variableName + "' non ha il token 'type'!");
    }

    nextToken(); // leggo cosa c'è dopo "type"
    if (s == "discrete") {
        if (log) std::cout << "Parser::parseVariable: rete di tipo \"discrete\", ok :)\n";
    } else {
        throw std::invalid_argument("Parser::parseVariable: la variabile non è di tipo \"discrete\"!");
    }

    while(nextToken() && s != "["); // aspetto "[" per numero di valori

    nextToken(); // leggo numero di valori
    int num_expected_values = std::stoi(s); // lo salvo come intero
    if (log) std::cout << "Parser::parseVariable: attesi N=" << num_expected_values << " valori\n";

    while(nextToken() && s != "{"); // aspetto la seconda "{" per la lista variabili

    std::string value;
    while (nextToken() && s != "}") {
        if(s == "," || s == "|" || s == "\t" || s == "\n" || s == "\r") continue;
        value = s;
        if (log) std::cout << "Parser::parseVariable: trovato valore \'" << value << "\'\n";

        newVariable.values.push_back(value);
    }

    if (num_expected_values != static_cast<int>(newVariable.values.size())) {
        throw std::runtime_error("Parser::parseVariable: errore nel parsing della variabile " + newVariable.name + ": il numero di valori indicati nel blocco \'variable\' non corrisponde con quelli effettivamente trovati.");
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
    while (nextToken() && s == "("); // salto eventuali parentesi, attendendo nome variabile

    std::string childName = s;
    int childId = id[childName];

    if (log) std::cout << "Parser::parseProbability: trovata variabile figlio \'" << childName << "\'\n";

    return childId;
}

void Parser::readParents(int childId) {
    while (nextToken()) {
        if (s == "|") { // ci sono genitori
            if (log) std::cout << ", ha genitori:";

            while (nextToken() && s != "{" && s != ")") { // ciclo su tutti i genitori
                if (s == "," || s == "\t" || s == "\n" || s == "\r") continue;

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
    while (nextToken()) {
        if (s == "{") continue;

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

    while (nextToken() && s != "}") {
        if (s == "," || s == ";" || s == "|" || s == "\t" || s == "\n" || s == "\r") continue;

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
            nextToken();
            while (s == "(" || s == "," || s == ";" || s == "|" || s == "\t" || s == "\n" || s == "\r") {
                nextToken();
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

            if (log) std::cout << " " << valueName << "(parentId=" << parentId << ",valueId=" << valueId << ")";
        }
        if (log) std::cout << std::endl;

        int cptRow = getCptRow(childId, partialAssignment);

        if (log) std::cout << "\tleggo probabilita ";

        // ciclo su ogni probabilità della riga
        for (int i = 0; i < numValues; i++) {
            nextToken();
            while (s == ")" || s == "," || s == ";" || s == "|" || s == "\t" || s == "\n" || s == "\r") nextToken();

            double prob = std::stod(s);
            if (log) std::cout << " " << prob;

            variables[childId].CPT[cptRow].push_back(prob);

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