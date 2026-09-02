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

### Query

Il metodo pubblico `query(targetId, evidence, customOrder)` prende in input:

- la variabile target;
- una eventuale mappa di evidenze;
- facoltativo: un ordine personalizzato di eliminazione.

ed esegue il calcolo della probabilità seguendo i seguenti passaggi:

1. Calcola l'insieme delle **variabili rilevanti = target + evidenze + loro antenati**, e salva un vettore di fattori che comprende solo quelli associati a queste variabili.

2. **Restrizione delle evidenze**: per ogni variabile di evidenza applica la funzione restrict ai fattori che la contengono.

3. **Ordine di eliminazione**: se non viene specificato un ordine di eliminazione in input (`customOrder`), costruisce un ordine contenente tutte le variabili rilevanti escludendo il target e le evidenze (default: ordine min-degree).

4. **Eliminazione** vera e propria. *Per ogni variabile*, nell'ordine di eliminazione:
    - separa i fattori che contengono la variabile da eliminare da quelli che non la contengono;
    - moltiplica i fattori che la contengono ottenendo un unico fattore combinato
    - applica `sumOut()` su questo fattore (elimina la variabile), aggiungendo il nuovo fattore ottenuto all'elenco dei fattori attivi.

5. **Moltiplicazione e normalizzazione**: moltiplica tutti i fattori attivi rimasti (contengono tutti solo il target) e applica `normalize()` per ottenere la distribuzione di probabilità finale.

Restituisce il fattore finale che contiene nella sua "table" la probabilità condizionata all'evidenza (o marginale se non è stata specificata l'evidenza) per ogni possibile valore del target.

### Funzioni statiche per calcolo delle probabilità

Sono implementate in `Elimination_probability.cpp` le seguenti funzioni statiche che permettono di calcolare le probabilità senza dover creare manualmente l'istanza di Elimination:

- `getConditionalProbability(network, variableName, valueName, evidenceNames)`:
    - **input**: la rete bayesiana, nome della variabile target, valore assegnato al target, evidenze rappresentate sottoforma di mappa di stringhe;
    - **output**: probabilità condizionale;
    - **cosa fa**: costruisce localmente l'istanza di Elimination associata alla rete in input, chiama la `query()` ed estrae dal fattore finale la probabilità richiesta.

- `getMarginalProbability(network, variableName, valueName)`:
    - **input**: la rete bayesiana, nome della variabile target, valore assegnato al target;
    - **output**: probabilità marginale;
    - **cosa fa**: chiama `getConditionalProbability()` senza specificare nessuna evidenza, calcolando quindi la probabilità marginale.

### Ordine di eliminazione min-degree

Se viene fatta partire la query senza specificare un ordine di eliminazione personalizzato, viene utilizzato l'ordine *min-degree*, restituito dalla funzione `minDegreeOrder(initialFactors, toEliminate)`.

**Funzionamento**: ad ogni passo, viene eliminata la variabile che, se eliminata ora, produce il fattore combined più piccolo (la dimensione viene approssimata guardando quante variabili vicine ha nel grafo, variabili vicine = se compaiono insieme nello scope di uno stesso fattore attivo) -> elimina la variabile con grado minimo (con meno vicini).

### Complessità

L'algoritmo di variable elimination ha una complessità temporale O(N * k^(w+1)).
Dove:
- N è il numero di variabili da eliminare;
- k è il numero di valori che una variabile può assumere (assumendo che tutte le variabili abbiano lo stesso numero di valori);
- w è il numero massimo di variabili presenti contemporaneamenete nello scope del fattore più grande generato.

Usare un'ordine di eliminazione min-degree permette di controllare la dimensione dei fattori, riducendo al minimo w -> si evita la complessità temporale esponenziale.