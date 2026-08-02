#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <string>
#include <vector>


struct Variable {
    std::string name; // nome della variabile
    std::vector<std::string> values; // possibili valori assunti dalla variabile
    std::vector<int> parents; // id dei genitori
    std::vector<std::vector<double>> CPT; // probabilità condizionata dai genitori
};

#endif