# Documento di progetto - Reti bayesiane

## Obiettivo

Il programma deve essere in grado di:
1. fare parsing di un file BIF che descrive una rete bayesiana discreta
2. costruire il DAG (grafo aciclico orientato) associato alla rete
3. calcolare la probabilità marginale di ciascuna variabile (tramite enumerazione completa), ordinata secondo ordinamento topologico del DAG

## Struttura base del progetto

Il progetto è suddiviso nei seguenti moduli:

- **Parser**: legge un file in formato BIF e salva le variabili della rete (nome, valori possibili, genitori, CPT).
- Classe **Network**:
    - rappresenta la rete bayesiana come DAG;
    - conserva le variabili della rete (rappresentate dalla struct **Variable** definita nel file *Variable.hpp*), le liste di adiacenza e l'ordinamento topologico;
    - contiene i getter per il calcolo delle probabilità marginali e condizionali.
- **Utilities**: funzioni di supporto generiche (es. stampa di vettori)

Estensioni / moduli aggiuntivi (da valutare più avanti):

- **Elimination**: implementerà eventualmente l'algoritmo di variable elimination per il calcolo delle probabilità (più efficiente dell'enumerazione completa).

## Stato di avanzamento

- [ ] costruzione della classe Network a livello base
- [ ] parsing file BIF
- [ ] ordinamento topologico
- [ ] calcolo probabilità congiunta
- [ ] probabilità marginale (con enumerazione completa)
- [ ] probabilità condizionale
- [ ] variable elimination

### Fase 1: strutture per rappresentare la rete bayesiana

- In questa prima fase costruisco la classe **Network** (*Network.hpp*, *Network.cpp*) a livello base, in particolare è dotata dei seguenti campi privati:
    - **variables**: vettore di elementi di tipo Variable che contiene tutte le variabili (i.e. i nodi) del network
    - la lista di adiacenza **adj**, costruita in modo che: adj[ID] contiene la lista dei figli di una variabile dato il suo ID;
    - **id**: una unordered map che associa ad ogni nome l'ID della variabile corrispondente;
- e dei seguenti metodi:
    - **addVariable**(variable): prende in input una reference a un oggetto del tipo Variable e lo aggiunge al network (facendo tutti i controlli necessari e aggiornando i campi).
- Il tipo **Variable** (definito in *Variable.hpp*) usato per descrivere i nodi è una struct con la seguente struttura:
    - **name** è il nome della variabile;
    - **values** sono i possibili valori che assume la variabile (esempio: *true*, *false*);
    - **parents** è un vettore che contiene gli ID dei genitori;
    - **CPT** (*Conditional Probability Table*) è la tabella che contiene le probabilità condizionate dai valori assunti dai genitori.

### Fase 2: parsing file BIF

- ...