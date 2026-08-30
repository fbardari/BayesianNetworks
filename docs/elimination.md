## Variable Elimination

- [Riferimento principale](https://www.youtube.com/playlist?list=PLdBx38JxhMNsJ4QcZ7OaIaSE1HBYNs-7u)

### Definizione dei fattori (classe `Factor`)

- Un fattore $F(X_1, ..., X_n)$ è una funzione che associa: ad ogni configurazione di un set di variabili ${X_1, ..., X_n}$ -> un numero.
- Saranno utilizzati rappresentare probabilità congiunte o condizionali.

Nell'header `Factor.hpp` è stata definita la classe `Factor` che descrive i fattori e fornisce le operazioni descritte di seguito.

### Operazioni sui fattori

#### Restrict

`factor.restrict(variableId, valueIndex)`

- Assegna un valore ad una variabile.
- **Output**: nuovo fattore $F(X_1=v_1, X_2, ..., X_n)$ con set di variabili $(X_2, ..., X_n)$

#### Sum out

`sumOut(variableId)`

Elimina una variabile sommando su tutti i possibili valori di quella variabile.

$$
\sum_{X_1} F(X_1, X_2, ..., X_n)
$$

#### Prodotto

`operator*(factor1, factor2)`

- Moltiplica due fattori.
- Il prodotto di due fattori $F_1(X,Y)$ e $F_2(Y,Z)$ dove Y è una variabile che hanno in comune è definito come

$$
(F_1 \times F_2) (X,Y,Z) = F_1(X,Y) * F_2(Y,Z)
$$

#### Normalizzazione

`factor.normalize()`

- Operazione che converte il fattore in una distribuzione di probabilità (ultimo step della variable elimination).
- Ogni valore è diviso per la somma di tutti i valori.

### Algoritmo di Variable Elimination (classe `Elimination`)

La classe `Elimination` (si vedano `Elimination.hpp` e `Elimination.cpp`) fornisce i metodi necessari per:
1. inizializzare correttamente i fattori leggendo nella tabella CPT di una rete;
2. eseguire una query (algoritmo di VE vero e proprio) per ottenere un fattore finale che contiene nel suo scope solo la variabile target, e leggerne le probabilità.

### Stato interno

- `net`: è un riferimento costante alla rete bayesiana su cui stiamo operando;

- `factors`: è un vettore che contiene i fattori (istanze di `Factor`) costruiti leggendo la CPT per ognuna delle variabili della rete.

### Funzioni helper e costruttore

- `toFactor(variableId)`: data una variabile del network (specificata dal suo ID), restituisce il fattore associato leggendo nella CPT i valori P(variabile | ...);

- `updateFactors()`: crea il fattore associato ad ogni variabile della rete e lo salva in `factors`;

- il costruttore prende in input una rete bayesiana e costruisce i fattori associati a tutte le sue variabili (chiamando `updateFactors()`, salvandoli quindi in `factors`).

### Query

Il metodo pubblico `query()` ...

### Funzioni statiche per calcolo delle probabilità

- `getConditionalProbability(network, variableName, valueName, evidenceNames)`:
    - **input**: la rete bayesiana, nome della variabile target, valore assegnato al target, evidenze rappresentate sottoforma di mappa di stringhe;
    - **output**: probabilità condizionale;
    - **cosa fa**: costruisce localmente l'istanza di Elimination associata alla rete in input, chiama la `query()` ed estrae dal fattore finale la probabilità richiesta.

- `getMarginalProbability(network, variableName, valueName)`:
    - **input**: la rete bayesiana, nome della variabile target, valore assegnato al target;
    - **output**: probabilità marginale;
    - **cosa fa**: chiama `getConditionalProbability()` senza specificare nessuna evidenza, calcolando quindi la probabilità marginale.





Per ogni variabile del network, la CPT viene convertita in un Factor:
- scope = variabile + i suoi genitori;
- table = valori della CPT.

Se stiamo calcolando la probabilità condizionale: P(X | E1=e1, E2=e2, ...), per ogni variabile di evidenza Ei e per ogni fattore che la contiene, facciamo:

F = F.restrict(Ei, ei)

Successivamente, eliminiamo tutte le variabili non target e non di evidenza (seguendo un ordine prefissato): per ciascuna variabile, moltiplichiamo tra di loro i fattori che la contengono e poi applichiamo il sumOut().

Alla fine, moltiplichiamo tutti i fattori rimasti (che contengono solo la variabile target X), normalizziamo il fattore risultante con normalize() e possiamo leggere le probabilità dal fattore ottenuto.