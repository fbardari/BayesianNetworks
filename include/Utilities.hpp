#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <vector>


// overload dell'operatore << per fare print di vettori generici
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    for (size_t i = 0; i < vec.size(); ++i)
    {
        os << vec[i] << (i + 1 == vec.size() ? "" : ", ");
    }
    return os;
}


#endif