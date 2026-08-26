# Documento di progetto - Reti bayesiane

## Obiettivo

Il programma deve essere in grado di:
1. fare parsing di un file BIF che descrive una rete bayesiana discreta
2. costruire il DAG (grafo aciclico orientato) associato alla rete
3. calcolare la probabilità marginale di ciascuna variabile (tramite enumerazione completa), ordinata secondo ordinamento topologico del DAG

## Struttura base del progetto

Il programma è suddiviso nei seguenti moduli:

- **Parser**: legge un file in formato BIF e salva le variabili della rete (nome, valori possibili, genitori, CPT).
- Classe **Network**:
    - rappresenta la rete bayesiana come **DAG** (*Directed Acyclic Graph*);
    - conserva le variabili della rete (rappresentate dalla struct **Variable** definita nel file *Variable.hpp*), le liste di adiacenza e l'ordinamento topologico;
    - contiene i getter per il calcolo delle probabilità marginali e condizionali.
- **Utilities**: funzioni di supporto generiche (es. stampa di vettori)
- **Test**: un namespace che contiene varie funzioni di test (es. check normalizzazione)
- **Menu**: gestisce l'interfaccia utente da riga di comando per permettere di:
    - caricare file BIF;
    - ispezionare le proprietà della rete;
    - calcolare probabilità congiunte e marginali (e in futuro implementerà eventualmente anche probabilità condizionale e VE);
    - aggiungere variabili al network.

Estensioni / moduli aggiuntivi (da valutare più avanti):

- **Elimination**: implementerà eventualmente l'algoritmo di variable elimination per il calcolo delle probabilità (più efficiente dell'enumerazione completa).

## Descrizione dettagliata dei moduli

### Variable

La struct **`Variable`** rappresenta i singoli nodi (variabili casuali discrete) che costituiscono la rete bayesiana:
- `name`: nome della variabile;
- `values`: vector che contiene i nomi dei valori (cioè gli stati discreti) che la variabile casuale può assumere (es. `["true", "false"]`);
- `parents`: vector che contiene gli ID dei genitori della variabile;
- `CPT`: tabella delle probabilità condizionate (*Conditional Probability Table*), memorizzata come matrice di valori di tipo `double`.

    *Nota sulle tabelle CPT*:

    - Le colonne corrispondono ai valori che la variabile può assumere (nell'esempio: d = true, false)
    - Le righe corrispondono ad un assignment dei valori delle variabili genitori (nell'esempio: b, c).

    ```
    probability (d | b, c) {
    (true, true) 0.9, 0.1;
    (true, false) 0.7, 0.3;
    (false, true) 0.6, 0.4;
    (false, false) 0.1, 0.9;
    }
    ```

    quindi, nel caso in esempio, la probabilità in posizione [0, 0] corrisponde a `P(d=true | b=true, c=true)`.
    
    La convenzione utilizzata per memorizzare i valori in `variable.CPT` è la seguente: le righe scorrono in modo che la variabile genitore più a destra sia quella con il valore che cambia più velocemente (nell'esempio: c = true/false/true/false, b = true/true/false/false, etc.).

    Il metodo `getCptRow(assignment)` (descritto nelle sezioni successive) permette di ricavare la riga dato un assignment dei valori seguendo questa convenzione.

### Network

La classe **`Network`** rappresenta la rete bayesiana e fornisce le funzioni per i calcoli delle probabilità marginali e congiunte.

#### Stato interno

Lo stato interno della classe **`Network`** contiene i seguenti membri privati:
- **`variables`**: vettore che contiene le variabili della rete (ordinate secondo gli ID assegnati);
- **`adj`**: liste di adiacenza che tracciano i figli di ogni variabile (cioè `adj[i]` contiene la lista dei figli della variabile con ID uguale ad i);
- **`id`**: *unordered map* che permette di trovare l'ID di una variabile dato il suo nome (es. `id['c']` -> 3);
    - *nota complessità*: unordered map è una tabella hash: la chiave, in questo caso la stringa, viene trasformata da una funzione di hashing nell'indice del bucket in cui cercare l'elemento; in questo modo la ricerca presenta una complessità *media* costante O(1) (costo del calcolo dell'hash), che può salire a O(n) nel *worst case scenario* (collisione tra chiavi).
- **`topologicalOrder`**: contiene gli ID delle variabili ordinate secondo l'ordine topologico.

I membri sopra indicati sono privati in quanto sono stati pensati per essere aggiornati automaticamente in modo *sicuro* e *coerente* ogni volta che aggiungiamo variabili alla rete, utilizzando gli appositi metodi pubblici. In questo modo si evitano a monte problemi come ID/nomi duplicati, genitori inesistenti, ciclicità della rete (si veda l'implementazione dei metodi seguenti, che sollevano le eccezioni opportune in caso di errore).

#### Metodi che modificano lo stato interno

Di seguito la descrizione dei metodi che modificano lo stato interno della classe (implementati in `Network.cpp`).

- **`addVariable(variable)`**
    - **input**: riferimento costante alla variabile (tipo: Variable) che si desidera aggiungere alla rete
    - **cosa controlla**:
        - che il nome della variabile non sia già preso
        - che gli ID dei genitori esistano
        - che la rete costruita aggiungendo la variabile non sia ciclica (indirettamente, tramite `updateTopologicalOrder()`)
    - **cosa fa**:
        - assegna un ID disponibile alla variabile;
        - aggiunge la nuova variabile al vettore *variables* (l'istanza contiene tutte le informazioni inclusa lista genitori tabella CPT);
        - modifica coerentemente `id` (associando il suo nome al suo ID) e `adj` (per ognuno dei suoi genitori, aggiunge questa variabile come figlio);
        - chiama `updateTopologicalOrder()` per aggiornare l'ordine topologico.

- **`updateTopologicalOrder()`**
    - **cosa controlla**: che la rete non sia ciclica.
    - **cosa fa**: costruisce l'ordine topologico mediante l'*algoritmo di Kahn*, cioè:
        - conta il numero di genitori di ogni variabile e inserisce in coda i nodi senza genitori
        - per ogni nodo, lo aggiunge all'ordinamento e decrementa - per ognuno dei suoi figli - il numero di genitori da processare
        - quando un figlio non ha più genitori da processare, viene inserito nella coda
        - se al termine non sono stati elaborati tutti i nodi, la rete contiene un ciclo -> in quel caso solleva un'eccezione
        - se sono stati elaborati tutti i nodi -> salva il risultato in `topologicalOrder`
    - **complessità**: il for iniziale ha complessità lineare O(n_nodi) nel numero di nodi, il ciclo while ha complessità lineare O(n_archi) nel numero di archi (per ogni nodo, un for processa tutti i genitori) -> complessità totale O(n_nodi + n_archi)

- **costruttore - default** `Network()`  
    - **cosa fa**: crea una rete vuota.

- **costruttore - overload con input vector di variabili** `Network(variables)`
    - **input**: riferimento costante ad un vector di variabili
    - **cosa fa**:
        - aggiunge tutte le variabili del vettore in input alla rete, facendo tutti i controlli necessari e gli update dello stato interno, esattamente come fa `addVariable(variable)`;
        - *solo alla fine* aggiorna ordine topologico chiamando `updateTopologicalOrder()` (sarebbe inutile fare *N_nodi* chiamate, chiamo il metodo una sola volta alla fine).

#### Metodi per il calcolo delle probabilità

I seguenti metodi calcolano le probabilità (sono implementati nel file `Network_probability.cpp`)

- funzione helper **`getCptRow(variable, assignment)`** oppure l'overload `getCptRow(variableId, assignment)`
    - **input**
        - variabile (passaggio per riferimento costante oppure direttamente il suo ID);
        - vettore di interi che contiene l'assignment per *tutte* le variabili della rete (esempio `a=true, b=false, c=true -> {1, 0, 1}`).
    - **output**: indice della riga della tabella CPT corrispondente a quell'assignment;
    - **cosa fa**: scorre sui genitori e, usando gli indici dei valori nell’assignment e un moltiplicatore basato sul numero di valori possibili, calcola e restituisce l’indice della riga corrispondente nella CPT (ogni genitore contribuisce a determinare "di quanti passi spostarsi" nella tabella);
    - **complessità**: un ciclo for visita ogni genitore eseguendo operazioni di complessità costante -> complessità totale O(N_genitori).

- `getJointProbability(assignment)`: calcola la probabilità congiunta data una configurazione completa di assegnamento

    - **input**: vettore di assignment (esempio `a=true, b=false, c=true -> {1, 0, 1}`);
    - **output**: probabilità congiunta dell'assignment (es. `P(a=true, b=false, c=true)`)
    - **cosa controlla**: che l'assignment abbia la dimensione corretta;
    - **cosa fa**: per ogni variabile individua nella CPT la probabilità corrispondente all'assignment (estraendo i valori dei suoi genitori dal vettore di assignment in input) e moltiplica tutte le probabilità ottenute;
    - **complessità**: ogni variabile viene visitata una volta, e per ogni variabile viene chiamata `getCptRow` che visita tutti i suoi genitori -> complessità totale O(N_variabili + N_archi).

- `getMarginalProbability(variableName, valueName)`: calcola la probabilità marginale di una specifica variabile e valore assegnato

    - **input**: nome variabile target e nome valore assegnato, es. `getMarginalProbability("e", "true")`
    - **output**: probabilità marginalizzata `P(e=true)`
    - **cosa fa**: *enumerazione completa*
        - usando `getAncestors` e `topologicalOrder` ricava le variabili *rilevanti*, cioè gli antenati ordinati topologicamente e la variabile target;
        - prepara un vettore di stati e associa ad ognuno di questi una probabilità
            - all'inizio abbiamo un solo stato con un valore dummy (-1) per tutte le variabili della rete, tranne che per la variabile target alla quale è assegnato il suo valore fissato;
            - la probabilità associata è fissata inizialmente a 1;
        - per ognuno degli antenati, il vettore degli stati viene ricostruito moltiplicando ogni stato in altri stati (uno per ogni valore possibile della variabile considerata), e la probabilità associata viene moltiplicata per il valore CPT corrispondente a quell'assignment, per esempio:
        ```
        Rete di esempio a -> b -> c -> ...
        Target P(c = false)

        [-1 -1 0 -1 ...] -> 1.0

        [0 -1 0 -1 ...] -> P(a = false)
        [1 -1 0 -1 ...] -> P(a = true)

        [0 0 0 -1 ...] -> P(a = false) * P(b = false | a = false)
        [0 1 0 -1 ...] -> P(a = false) * P(b = true | a = false)
        [1 0 0 -1 ...] -> P(a = true) * P(b = false | a = true)
        [1 1 0 -1 ...] -> P(a = true) * P(b = true | a = true)
        ```
        - una volta arrivati al target ogni stato resta identico, ma la probabilità viene comunque moltiplicata per il fattore CPT corrispondente al valore fissato (nell'esempio precedente, l'ultimo passo moltiplicherebbe ognuno dei 4 stati per `P(c = false | b = ...)`, con `b` preso dallo stato stesso);
        ```
        [0 0 0 -1 ...] -> P(a = false) * P(b = false | a = false) * P(c = false | b = false)
        [0 1 0 -1 ...] -> P(a = false) * P(b = true | a = false) * P(c = false | b = true)
        [1 0 0 -1 ...] -> P(a = true) * P(b = false | a = true) * P(c = false | b = false)
        [1 1 0 -1 ...] -> P(a = true) * P(b = true | a = true) * P(c = false | b = true)
        ```
        - la somma di tutte le probabilità ottenute è per definizione la probabilità marginalizzata
    - **complessità**: il costo computazionale dell'algoritmo utilizzato è O(m * k^m), dove k è il numero di valori possibili (assumendo che sia uguale per tutte le variabili), ed m è il numero di variabili rilevanti.


#### Altri metodi che NON modificano lo stato interno

Di seguito la descrizione di altri metodi pubblici della classe Network che NON modificano lo stato interno della classe (implementati in `Network.cpp`). 

- `size()`: restituisce il numero di variabili del network;
- `getNames()`: restituisce i nomi di tutte le variabili (vector di stringhe);
- `arcsCount()`: restituisce il numero di archi del grafo;
- `getVariableId(name)`: dato in input il nome di una variabile, restituisce il suo ID, accedendo alla unordered map "id";
- `getChildren(id)`: dato l'ID di una variabile, restituisce un riferimento costante ad un vettore che contiene gli ID dei suoi figli, accedendo alle liste di adiacenza;
- `getTopologicalOrder()`: getter per topologicalOrder;
- `getValues(variableName)`: dato in input il nome di una variabile, restituisce un riferimento costante ad un vettore di stringhe contenente i nomi valori possibili (esempio: getValues("a") -> {"true", "false"});
- `getValueIndex(variableId, valueName`): dati in input l'ID di una variabile e il nome di un valore possibile, restituisce l'ID di quel valore (i.e. l'indice in corrispondenza del quale il valore è memorizzato in variable.values)
    - *complessità*: esegue ricerca lineare in variable.values -> O(n_values);
- `getAncestors(variableId)`: dato in input l'ID di una variabile, restituisce un unordered_set che contiene l'insieme di *tutti* gli antenati di quella variabile (cioè quelli che sono rilevanti per il calcolo della probabilità marginale)
    - *algoritmo implementato*: ricerca in ampiezza (BFS)
        1. parte dalla variabile in input e la mette in coda
        2. ad ogni iterazione estrae un nodo u dalla coda e scorre i suoi genitori (variables[u].parents).
        3. per ogni genitore, prova ad inserirlo in visited: se l'inserimento ha successo (cioè non era già presente), lo aggiunge anche alla coda, per poi esplorare a sua volta i suoi genitori;
        4. il ciclo continua finché la coda non si svuota, cioè quando abbiamo risalito il grafo visitando tutti gli antenati.
    - *complessità*
        - caso migliore: O(1), quando la variabile non ha genitori;
        - caso peggiore: O(n_antenati + archi tra gli antenati), cioè quando la variabile dipende da tutta la rete;
        - caso medio: dipende dalla struttura della rete, non si può determinare a priori.
- overload di `operator[]`: in modo che `network[ID]` restituisca una reference alla variabile con quell'ID.

## Stato di avanzamento

1. [x] costruzione della classe Network a livello base
    - [x] capacità di leggere le probabilità dalla CPT: -> fatto: implementato il metodo network.**getCptRow**(variable, assignment) che calcola l'**indice di riga della matrice CPT** *dato un assignment set COMPLETO dei genitori* di una certa variabile
        - **Funzionamento di getCptRow()**: prende gli index dei valori scelti da ciascun genitore, li moltiplica per pesi crescenti in base al numero di opzioni disponibili e somma per trovare la riga esatta (nella tabella CPT le righe sono ordinate in modo che l'ultimo genitore abbia il valore che varia più velocemente)
        - input: (variabile, assignment) oppure (ID variabile, assignment)
2. [x] ordinamento topologico
3. [x] calcolo probabilità congiunta e marginale
    - [x] costruire la funzione `network.getJointProbability(assignment)` che calcola la probabilità di una configurazione completa
    - [x] generare tutte le configurazioni delle variabili rilevanti per la variabile considerata
    - [x] sommare solo su quelle in cui la variabile assume il valore fissato a priori
    - [x] **DA MODIFICARE**: attualmente, nel calcolo della probabilità marginale per enumerazione completa sto calcolando tutte le configurazioni complete -> devo invece generare solo le configurazioni delle variabili da cui la variabile target dipende!
4. [x] parsing file BIF
    - [x] deve essere capace di leggere i blocchi variable
    - [x] e di popolare le tabelle CPT leggendo i blocchi probability
    - ~~[ ] TODO: ottimizzare firma della funzione helper **Parser::getCptRow()**, non necessario che sia static, non necessario passaggio per riferimento~~ *rimossa inutile complicazione*
5. [x] eventuale interfaccia utente
6. [ ] probabilità condizionale
7. [ ] variable elimination

### Parser

#### Stato interno

Il **`Parser`** è definito come una classe il cui stato interno comprende:
- la stringa `filename`: il percorso del file BIF che il Parser sta leggendo;
- l'oggetto di tipo `std::fstream` "`file`";
- la stringa `s` con la quale i metodi in cui si smista il parsing salvano e si passano l'ultimo token letto;
- la variabile booleana `log` con la quale decidiamo se printare il log durante il parsing (utilizzato durante lo sviluppo per vedere a colpo d'occhio eventuali bug nella lettura dei token).

Contiene inoltre la unordered map (string -> int) `id` e il vettore di variabili `variables`, che andranno a costruire il nuovo oggetto di tipo Network una volta completato il parsing.

#### Funzionamento del parsing

Il metodo pubblico **`parse()`** avvia il parsing sul file BIF indicato in `filename`, restituendo l'oggetto di tipo Network costruito con i dati letti.

Contiene un ciclo for che legge token dal file finché il file non è terminato, rimuovendo dalla stringa i caratteri speciali con la funzione helper `cleanString()`.
Quando la stringa letta corrisponde a una delle intestazioni dei blocchi (network, variable o probability) -> avvia il ciclo corrispondente:
- parseNetwork();
- parseVariable(): legge le variabili e inizia a riempire il vettore `variables`;
- parseProbability(): legge, per ogni blocco probability, i genitori della variabile e la tabella CPT, chiamando i metodi opportuni come indicato di seguito:
    ```
    Parser::parse()
    ├── parseNetwork()
    ├── parseVariable()
    └── parseProbability()
        ├── readProbabilityChild()
        ├── readParents()
        └── readCptTable()
            ├── readCptTableNoParents()
            └── readCptRows()
                └── getCptRow()
    ```
- `getCptRow()` viene utilizzata per assegnare ogni valore alla riga corretta della tabella CPT, coerentemente con la convenzione decisa precedentemente.

#### Accesso dall'esterno

- Il metodo statico `Parser::importBIF(filename, log)` crea localmente un parser usa e getta che legge il file indicato in input e restituisce l'oggetto di tipo Network corrispondente.

### Utilities

L'header `Utilities.hpp` contiene gli overload dell'operatore <<, per permettere di fare print di vettori e matrici.

### Test

Il namespace `Test` contiene alcune funzioni utili per verificare il corretto funzionamento del programma:
- `Test::exampleNetwork()` restituisce la rete del file gradient.bif, creata manualmente (funzione utilizzata per verificare il corretto funzionamento del parser);
- `Test::normalized(network)` restituisce true solo se tutte le probabilità marginali del network in input sono normalizzate a 1.0 (a meno del machine epsilon).

### Fase 1: strutture per rappresentare la rete bayesiana

In questa fase sono state sviluppate le strutture dati utilizzate per rappresentare la rete bayesiana, ora descritte più dettagliatamente nelle sezioni [Network](#Network) e [Variable](#Variable).

Alcune scelte implementative:

- gli ID, le righe della tabella CPT e gli indici di ogni genere sono rappresentati come interi (tipo standard `int`), non è stato necessario utilizzare gli interi a 64 bit;
- è stato aggiunto in seguito lo `static_cast<int>` quando risultati di tipo `size_t` venivano confrontati con interi standard (link ai commit [1](https://github.com/fbardari/BayesianNetworks/commit/d571b292bb5e1565bd8b939254def86c7f6a647f) [2](https://github.com/fbardari/BayesianNetworks/commit/9cf37e70042eb91d3d358ed6e795095c29c3882f)).

### [metodo abbandonato] ~~Fase 2:~~ calcolo della probabilità marginale (enumerazione completa)

Si rimanda al file [design_old](./design_old.md) per l'algoritmo di enumerazione completa per ricorrenza, abbandonato in quanto di complessità esponenziale nel numero complessivo di *variabili*, anziché nel numero di *variabili rilevanti* (-> sezione successiva).

### [nuovo metodo] Fase 2: ottimizzazione del calcolo della probabilità marginale

In seguito è stato riscritto l'algoritmo ([link al commit](https://github.com/fbardari/BayesianNetworks/commit/29ee02c8bba7f765bb104447b1e25b22a21b93ae)), abbandonando la ricorsione, e utilizzando l'approccio descritto ora nella sezione [Metodi per il calcolo delle probabilità](#Network).

### Fase 3: parsing file BIF

È stato costruito il Parser nelle seguenti fasi:
1. inizialmente soltanto la struttura base del ciclo in parse() [link al commit](https://github.com/fbardari/BayesianNetworks/commit/6720b6e32812cb732b97a3221475ca8dd57edbac);
2. poi il parsing del blocco variable (https://github.com/fbardari/BayesianNetworks/commit/128de88d3c1d98f6b597189349983ecc7742a22c);
3. infine il parsing del blocco probability [link al commit]()
    - separate in seguito le funzioni che fanno parsing del sottoblocchi che compongono il blocco "probability" per migliorare la leggibilità del codice (link ai commit più significativi [1](https://github.com/fbardari/BayesianNetworks/commit/7a529eced4a21e8935495d131ccd5b2d9cecd230) [2](https://github.com/fbardari/BayesianNetworks/commit/1aa6e0342fbd4c7e1aaa5989621eca9d7dbb995e)).

Per testare il corretto funzionamento del Parser è stato confrontato il network ottenuto facendo parsing dal file gradient.bif con quello generato manualmente a partire dalla stessa rete ([link al commit](https://github.com/fbardari/BayesianNetworks/commit/eb287db0620f5b6718b4cd068056182481b2e378)).

La descrizione dettagliata del Parser si trova adesso nella sezione [Parser](#Parser).

### Fase 4: probabilità condizionale

...

### Fase 5: variable elimination
...