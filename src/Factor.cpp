#include "Factor.hpp"
#include <stdexcept>
#include <numeric>
#include <cmath>

int Factor::indexOf(int variableId) const {
    for (size_t i = 0; i < scope.size(); ++i) {
        if (scope[i] == variableId) return static_cast<int>(i);
    }
    return -1;
}

bool Factor::contains(int variableId) const { 
    return indexOf(variableId) != -1; 
}

int Factor::getTableIndex(const std::vector<int>& assignment) const {
    int index = 0, stride = 1;
    for (int i = static_cast<int>(scope.size()) - 1; i >= 0; --i) {
        index += assignment[i] * stride;
        stride *= numValues[i];
    }
    return index;
}

std::vector<int> Factor::getAssignment(int flatIndex) const {
    std::vector<int> assignment(scope.size());
    for (int i = static_cast<int>(scope.size()) - 1; i >= 0; --i) {
        assignment[i] = flatIndex % numValues[i];
        flatIndex /= numValues[i];
    }
    return assignment;
}



Factor Factor::restrict(int variableId, int valueIndex) const {
    int pos = indexOf(variableId);
    if (pos == -1) return *this;

    Factor res;
    for (size_t i = 0; i < scope.size(); ++i) {
        if (static_cast<int>(i) != pos) {
            res.scope.push_back(scope[i]);
            res.numValues.push_back(numValues[i]);
        }
    }

    if (res.scope.empty()) {
        std::vector<int> singleAssign(scope.size());
        singleAssign[pos] = valueIndex;
        res.table.push_back(table[getTableIndex(singleAssign)]);
        return res;
    }

    int totalSize = 1;
    for (int nv : res.numValues) totalSize *= nv;
    res.table.resize(totalSize);

    std::vector<int> oldAssign(scope.size());
    oldAssign[pos] = valueIndex;

    for (int i = 0; i < totalSize; ++i) {
        auto newAssign = res.getAssignment(i);
        
        for (size_t j = 0, k = 0; j < scope.size(); ++j) {
            if (static_cast<int>(j) != pos) {
                oldAssign[j] = newAssign[k++];
            }
        }
        res.table[i] = table[getTableIndex(oldAssign)];
    }
    return res;
}

Factor Factor::sumOut(int variableId) const {
    int pos = indexOf(variableId);
    if (pos == -1) return *this;

    Factor res;
    for (size_t i = 0; i < scope.size(); ++i) {
        if (static_cast<int>(i) != pos) {
            res.scope.push_back(scope[i]);
            res.numValues.push_back(numValues[i]);
        }
    }

    if (res.scope.empty()) {
        double totalSum = 0.0;
        for (double val : table) totalSum += val;
        res.table.push_back(totalSum);
        return res;
    }

    int newSize = 1;
    for (int nv : res.numValues) newSize *= nv;
    res.table.assign(newSize, 0.0);

    for (size_t i = 0; i < table.size(); ++i) {
        auto oldAssign = getAssignment(static_cast<int>(i));

        std::vector<int> newAssign;
        newAssign.reserve(res.scope.size());
        for (size_t j = 0; j < oldAssign.size(); ++j) {
            if (static_cast<int>(j) != pos) {
                newAssign.push_back(oldAssign[j]);
            }
        }

        res.table[res.getTableIndex(newAssign)] += table[i];
    }

    return res;
}




Factor operator*(const Factor& f1, const Factor& f2) {
    // Gestione casi limite con tabelle vuote
    if (f1.table.empty()) return f2;
    if (f2.table.empty()) return f1;

    Factor res = f1;

    std::vector<int> f2ToRes;
    for (size_t i = 0; i < f2.scope.size(); ++i) {
        int pos = res.indexOf(f2.scope[i]);
        if (pos == -1) {
            res.scope.push_back(f2.scope[i]);
            res.numValues.push_back(f2.numValues[i]);
            pos = static_cast<int>(res.scope.size()) - 1;
        }
        f2ToRes.push_back(pos);
    }

    int totalSize = 1;
    for (int nv : res.numValues) totalSize *= nv;
    res.table.resize(totalSize);

    std::vector<int> assign1(f1.scope.size());
    std::vector<int> assign2(f2.scope.size());

    for (int i = 0; i < totalSize; ++i) {
        auto resAssign = res.getAssignment(i);

        for (size_t j = 0; j < f1.scope.size(); ++j) {
            assign1[j] = resAssign[j];
        }

        for (size_t j = 0; j < f2.scope.size(); ++j) {
            assign2[j] = resAssign[f2ToRes[j]];
        }

        double val1 = f1.scope.empty() ? f1.table[0] : f1.table[f1.getTableIndex(assign1)];
        double val2 = f2.scope.empty() ? f2.table[0] : f2.table[f2.getTableIndex(assign2)];
        res.table[i] = val1 * val2;
    }

    return res;
}


void Factor::normalize() {
    double sum = 0.0;
    for (double val : table) {
        sum += val;
    }

    if (std::abs(sum) > 1e-12) {
        for (double& val : table) {
            val /= sum;
        }
    }
}