#include "Menu.hpp"

Menu::Menu() = default;

Menu::Menu(const std::string& fileNameInput) {
    try {
        loadFile(fileNameInput);
    } catch (const std::runtime_error& e) {
        clearScreen();
        std::cout << "\n > Errore nel caricamento del file";
        pause();
        loaded = false;
    }
}

void Menu::clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void Menu::pause() {
    std::cout << "\n\n > Premere invio per continuare\n";
    std::cin.get();
    clearScreen();
}

void Menu::loadFile(std::string fileNameInput) {
    fileName = fileNameInput;
    network = Parser::importBIF(fileName, log);
    loaded = true;

    if (log) pause();
}

void Menu::run() {
    while (!exitCmd) mainMenu();
}

void Menu::margMenu() {
    clearScreen();
    std::cout << "\n > CALCOLO PROBABILITÀ MARGINALE (enumerazione completa)\n";

    try {
        std::cout << "\n > Variabili: " << network.getNames();

        std::string variableName, valueName;

        std::cout << "\n > Inserisci il nome di una variabile\n > ";
        std::getline(std::cin, variableName);

        std::cout << "\n > Valori possibili: " << network.getValues(variableName);

        std::cout << "\n > Inserisci il nome di un valore\n > ";
        std::getline(std::cin, valueName);

        double prob = network.getMarginalProbability(variableName, valueName);
        std::cout << "\n > Probabilità: " << prob;
    } catch (const std::invalid_argument& e) {
        std::cout << "\n\n > Errore: " << e.what();
    }
    pause();
}

void Menu::jointMenu() {
    clearScreen();
    std::cout << "\n\n > CALCOLO JOINT PROBABILITY\n\n";

    int size = network.size();
    std::vector<int> assignment(size);
    std::string input;

    for (int i = 0; i < size; i++) {
        while (true) {
            std::cout << " > Scegliere il valore di \"" << network[i].name
                       << "\" tra i possibili valori: " << network[i].values << "\n > ";
            std::getline(std::cin, input);

            try {
                assignment[i] = network.getValueIndex(i, input);
            } catch (const std::invalid_argument& e) {
                std::cout << " > Errore nel valore inserito, riprovare\n\n";
                continue;
            }
            std::cout << "\n";
            break;
        }
    }

    double prob = network.getJointProbability(assignment);
    std::cout << " > Probabilità della configurazione = " << prob;
    pause();
}

void Menu::addVariableMenu() {
    clearScreen();
    std::cout << "\n > AGGIUNGI VARIABILE\n";

    std::string input;
    Variable variable;

    std::cout << "\n > Nome variabile\n > ";
    std::getline(std::cin, input);
    variable.name = input;

    do {
        std::cout << "\n > Nome valore (oppure premere invio)\n > ";
        std::getline(std::cin, input);
        if (!input.empty()) variable.values.push_back(input);
    } while (!input.empty());

    do {
        std::cout << "\n > Nome genitore (oppure premere invio)\n > ";
        std::getline(std::cin, input);
        if (input.empty()) break;

        try {
            variable.parents.push_back(network.getVariableId(input));
        } catch (const std::invalid_argument& e) {
            std::cout << " > Genitore non valido, riprovare" << std::endl;
        }
    } while (true);

    std::cout << std::endl;

    int numRows = 1;
    for (int parentId : variable.parents) numRows *= network[parentId].values.size();
    int numColumns = static_cast<int>(variable.parents.size());

    for (int row = 0; row < numRows; row++) {
        std::cout << "> Riga " << row << ": [ ";
        int tempRow = row;
        std::vector<std::string> combination(numColumns);

        for (int column = 0; column < numColumns; column++) {
            int parentId = variable.parents[column];
            int numStates = static_cast<int>(network[parentId].values.size());
            combination[column] = network[parentId].values[tempRow % numStates];
            tempRow /= numStates;
        }

        for (int i = 0; i < numColumns; i++) {
            std::cout << network[variable.parents[i]].name << "=" << combination[i]
                       << (i == numColumns - 1 ? "" : ", ");
        }
        std::cout << " ]" << std::endl;

        std::vector<double> probRow;
        for (const std::string& val : variable.values) {
            double p;
            while (true) {
                std::cout << " > P(" << variable.name << " = " << val << ") = ";
                if (std::cin >> p) {
                    probRow.push_back(p);
                    break;
                }
                std::cout << " > Errore: inserito valore non valido, riprovare\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        variable.CPT.push_back(probRow);
        std::cout << " > Riga completata\n\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // pulisco newline residuo da std::cin >> p

    try {
        network.addVariable(variable);
    } catch (const std::invalid_argument& e) {
        std::cout << " > Errore: " << e.what() << "\n > La variabile non è stata salvata\n";
    }
    pause();
}

void Menu::topologicalOrderMenu() {
    clearScreen();
    std::cout << "\n > Variabili del network in ordine topologico\n\n";

    int counter = 1;
    for (int id : network.getTopologicalOrder()) {
        std::cout << "   " << counter++ << ". " << network[id].name << ", ID = " << id << "\n";
    }
    pause();
}

void Menu::resetMenu() {
    clearScreen();
    std::string input;

    std::cout << "\n\n > Stai per cancellare tutte le modifiche fatte alla rete, confermi di voler procedere?\n";
    std::cout << " > Digitare 'confermo' per procedere (oppure premere invio per annullare)\n\n > ";
    std::getline(std::cin, input);

    if (input == "confermo") loaded = false;
}

void Menu::mainMenu() {
    clearScreen();
    std::cout << std::endl;
    std::cout << "\n > MENU PRINCIPALE\n\n > ";

    std::string input;

    if (loaded) {
        std::cout << "File caricato " << fileName << "\n\n";

        std::cout << " > show = mostra riassunto del network\n";
        std::cout << " > topological = mostra variabili in ordine topologico\n";
        std::cout << " > size = mostra dimensioni\n";
        std::cout << " > marg = calcola una probabilità marginale (enumerazione completa)\n";
        std::cout << " > joint = calcola probabilità data una configurazione completa\n\n";
        std::cout << " > " << (log ? "log = disattiva log\n" : "log = attiva log\n");
        std::cout << " > add = aggiungi nuova variabile\n";
        std::cout << " > load = carica nuovo file\n";
        std::cout << " > exit = uscire dal programma\n";
        std::cout << "\n > Inserire comando\n > ";

        std::getline(std::cin, input);
    } else {
        std::cout << "Nessun file caricato\n\n";
        std::cout << " > Inserire il percorso del file\n";
        std::cout << " > Digitare 'exit' per uscire\n";
        std::cout << " > Digitare 'empty' per creare una rete vuota\n\n > ";

        std::getline(std::cin, input);

        if (input == "empty") {
            fileName = "(rete vuota)";
            network = Network();
            loaded = true;
        } else if (input != "exit") {
            try {
                loadFile(input);
            } catch (const std::runtime_error& e) {
                std::cout << " > File non valido, riprovare :(" << std::endl;
                pause();
            }
        }
    }

    if (input == "exit") {
        clearScreen();
        std::cout << "\n > Esco dal programma, ciao :)\n\n";
        exitCmd = true;
    } else if (input == "show") {
        clearScreen();
        std::cout << network;
        pause();
    } else if (input == "load") {
        resetMenu();
    } else if (input == "marg") {
        margMenu();
    } else if (input == "size") {
        clearScreen();
        std::cout << "\n > Il network caricato ha " << network.size()
                   << " variabili e " << network.arcsCount() << " archi.";
        pause();
    } else if (input == "log") {
        log = !log;
    } else if (input == "add") {
        addVariableMenu();
    } else if (input == "topological") {
        topologicalOrderMenu();
    } else if (input == "joint") {
        jointMenu();
    }
}