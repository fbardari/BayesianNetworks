#include "Network.hpp"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Variable& variable) {

    os << "\n\nVARIABLE INFORMATIONS\n\n";

    os << "Name: " << variable.name;

        // print PARENTS
        if (variable.parents.empty()) {
            os << "\nParents:\n   None\n";
        } else {
            os << "\nParents:\n";
            for (int parentId : variable.parents) {
                os << " - ID: " << parentId << std::endl;
            }
        }

        // print VALUES
        os << "Values:\n";
        for (std::string value : variable.values) {
            os << " - " << value << std::endl;
        }
        os << "\n";

    return os;
}

std::ostream& operator<<(std::ostream& os, const Network& network) {
    os << "\n\nInfo Network (" << network.size() << " variables)\n\n";

    for (int i = 0; i < network.size(); i++) {
        os << "ID: " << i << ", Nome: " << network[i].name;

        // print CHILDREN
        if (network.getChildren(i).empty()) {
            os << "\nFigli:\n\tNone\n";
        } else {
            os << "\nFigli:\n";
            for (int childId : network.getChildren(i)) {
                os << " - ID: " << childId << ", Nome: " << network[childId].name << "\n";
            }
        }

        // print PARENTS
        if (network[i].parents.empty()) {
            os << "Genitori:\n\tnessuno\n";
        } else {
            os << "Genitori:\n";
            for (int parentId : network[i].parents) {
                os << " - ID: " << parentId << ", Nome: " << network[parentId].name << "\n";
            }
        }

        // print VALUES
        os << "Valori:\n";
        for (std::string value : network[i].values) {
            os << " - " << value << std::endl;
        }
        os << "\n";
    }
    return os;
}