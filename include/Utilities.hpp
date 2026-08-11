#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <vector>


// overload dell'operatore << per fare print di vettori generici
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i] << (i + 1 == vec.size() ? "" : "\t");
    }
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<std::vector<T>> &mat) {
    for (const auto &row : mat) {
        os << row << '\n';
    }
    return os;
}


#endif