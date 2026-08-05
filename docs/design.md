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
- **Test**: un namespace che contiene varie funzioni di test (es. check normalizzazione)

Estensioni / moduli aggiuntivi (da valutare più avanti):

- **Elimination**: implementerà eventualmente l'algoritmo di variable elimination per il calcolo delle probabilità (più efficiente dell'enumerazione completa).

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
4. [ ] probabilità condizionale
5. [ ] parsing file BIF
    - [ ] deve essere capace di leggere i blocchi variable
    - [ ] e di popolare le tabelle CPT leggendo i blocchi probability
6. [ ] variable elimination
7. [ ] eventuale interfaccia utente

### Fase 1: strutture per rappresentare la rete bayesiana

- Il tipo **Variable** (definito in *Variable.hpp*) usato per descrivere i nodi è una struct con la seguente struttura:
    - **name** è il nome della variabile;
    - **values** sono i possibili valori che assume la variabile (esempio: *true*, *false*);
    - **parents** è un vettore che contiene gli ID dei genitori;
    - **CPT** (*Conditional Probability Table*) è la tabella che contiene le probabilità condizionate dai valori assunti dai genitori.
- In questa prima fase costruisco la classe **Network** (*Network.hpp*, *Network.cpp*) a livello base, in particolare è dotata dei seguenti costruttori
    - Network(): costruttore standard, crea una rete vuota;
    - Network(variables): costruisce una rete partendo da un vettore di oggetti di tipo Variable.
- dei seguenti campi privati:
    - **variables**: vettore di elementi di tipo Variable che contiene tutte le variabili (i.e. i nodi) del network
    - la lista di adiacenza **adj**, costruita in modo che: adj[ID] contiene la lista dei figli di una variabile dato il suo ID;
    - **id**: una unordered map che associa ad ogni nome l'ID della variabile corrispondente;
    - **topologicalOrder**: vettore che contiene gli ID delle variabili ordinate secondo l'ordine topologico;
- e dei seguenti metodi:
    - **addVariable**(variable): prende in input una reference a un oggetto del tipo Variable e lo aggiunge al network (facendo tutti i controlli necessari e aggiornando i campi, incluso l'ordine topologico);
    - **updateTopologicalOrder**(): aggiorna l'ordine topologico (viene chiamato ogni volta che viene aggiunta una variabile) (-> inoltre è in grado di controllare se il network è ciclico, sollevando eventualmente un'eccezione).
- aggiungo in questa fase dei getter che potrebbero essere utili nelle fasi successive:
    - network.**size**() restituisce il numero di variabili (nodi) del network;
    - network.**getTopologicalOrder**() restituisce il vettore che contiene gli ID delle variabili in ordine topologico;
    - network.**getValues**(variableName): nome variabile -> nomi valori possibili (es. *true/false*);
    - network.**getVariableId**(name): nome variabile -> ID (in altre parole, accesso in sola lettura alla mappa **id**);
    - network.**getChildren**(id): getter per liste di adiacenza, id genitore -> vettore con id dei figli.
    - network.**getValueIndex**(variableId, valueName): (ID variabile, nome valore) -> ID valore (per esempio "true" = 0, "false" = 1 ...);
    - **network\[variableId\]** = overload dell'operatore [] che restituisce una reference alla variabile con l'ID specificato.

### Fase 2: calcolo della probabilità marginale (enumerazione completa)

- Abbiamo adesso le strutture per rappresentare il network e le CPT.
- Abbiamo una funzione (**getJointProbability**) che dato un assignment completo ci da la probabilità congiunta di quella specifica configuazione (es. *p(a=false, b=true, c=true...)*)
- Adesso dobbiamo creare la funzione che marginalizza la probabilità (es. *p(b=true)*) per **enumerazione completa**, cioè calcolando la probabilità (congiunte) di tutte le configurazioni complete *rilevanti* (cioè, per esempio, in cui *b=true*) e sommandole.
- La probabilità marginale sarà calcolata dalla funzione **getMarginalProbability** che prende in input due stringhe: il nome di una variabile target e il nome del suo valore (es. `getMarginalProbability("b", "true")`).
    - **getMarginalProbability** trova innanzitutto l'ID della variabile target e del valore assegnato
    - crea un array assignment che è inizializzato tutto ad un valore dummy (-1) tranne per l'elemento corrispondente alla variabile target, che inizializzato al valore assegnato (es. true)
    - a questo punto viene chiamata la funzione ricorsiva **marginalRecursive** che prende in input l'ID di una variabile da processare e un assignment completo
- **Funzionamento di marginalRecursive()**: distinguendo i vari casi
    - se abbiamo processato l'ultima variabile (ID variabile da processare = network size) -> restituisce la joint probability;
    - se la variabile è stata già processata (assignment[id] != -1) -> chiama l'algoritmo ricorsivo per la variabile successiva "id+1";
    - altrimenti (la variabile non ha ancora un valore, ovvero assignment[id] = -1):
        - inizializza probabilità a 0.0;
        - itera su tutti i possibili valori che la variabile corrente può assumere, assegna il valore corrente a assignment[id], fa chiamata ricorsiva per la variabile successiva e somma il risultato alla probabilità
[TODO: spiegare meglio]

### Fase 3: parsing file BIF

- In generale, il modulo Parser deve contenere una funzione del tipo **importBIF(filePath)** che dato il pecorso di un file BIF restituisce l'oggetto di tipo Network costruito secondo i dati trovati nel file.