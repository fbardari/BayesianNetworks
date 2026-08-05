#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <vector>


// overload dell'operatore << per fare print di vettori generici
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<std::vector<T>> &mat) {
    for (const auto &row : mat) {
        for (size_t j = 0; j < row.size(); ++j)
            os << row[j] << (j + 1 == row.size() ? "" : "\t");
        os << '\n';
    }
    return os;
}


#endif