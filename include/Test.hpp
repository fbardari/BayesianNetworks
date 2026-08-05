#ifndef TEST_HPP
#define TEST_HPP

#include "Network.hpp"

namespace Test {

/*
costruisce manualmente il network di esempio
del file gradient.bif
*/ 
Network exampleNetwork();

/*
verifica, per ogni variabile,
che le probabilità marginali siano normalizzate
a meno del machine epsilon

es. P(a=true) + P(a=false) = 1.0
*/
bool normalized(Network& network);

};


#endif