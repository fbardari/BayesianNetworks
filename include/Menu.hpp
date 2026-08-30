#ifndef MENU_HPP
#define MENU_HPP

#include <cstdlib>
#include <iostream>
#include <string>

#include "Network.hpp"
#include "Utilities.hpp"
#include "Parser.hpp"
#include "Test.hpp"

class Menu {

private:

Network network; // network
std::string fileName; // nome del file importato

void loadFile(std::string fileNameInput); // importa file
bool loaded = false; // abbiamo importato un file?

bool log = false; // voglio print dei log?
bool exitCmd = false; // è stato dato il comando di uscire dal programma?

void mainMenu(); // menu principale
void margMenu(); // menu per calcolo probabilità marginale
void jointMenu(); // menu per calcolo joint probability
void topologicalOrderMenu(); // mostra variabili in ordine topologico
void addVariableMenu(); // menu aggiunta variabile
void resetMenu(); // menu che chiede conferma prima di resettare il network caricato
void veMargMenu();
void veCondMenu();

void clearScreen(); // pulisce lo schermo
void pause(); // premi invio per menu principale

public:

Menu(); // crea il menu (senza caricare file)
Menu(const std::string& fileNameInput); // crea il menu (caricando file in input)

void run(); // avvia l'interfaccia utente

};

#endif