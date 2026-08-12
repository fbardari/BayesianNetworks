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

## Descrizione dettagliata dei moduli

### Variable.hpp

La struct **`Variable`** rappresenta i singoli nodi (variabili casuali discrete) che costituiscono la rete bayesiana:
- `name`: nome della variabile;
- `values`: vector che contiene i nomi dei valori (cioè gli stati discreti) che la variabile casuale può assumere (es. `["true", "false"]`);
- `parents`: vector che contiene gli ID dei genitori della variabile;
- `CPT`: tabella delle probabilità condizionate (*Conditional Probability Table*), memorizzata come matrice di valori di tipo `double`.

### Network.hpp

La classe **`Network`** rappresenta la rete bayesiana e fornisce le funzioni per i calcoli delle probabilità marginali e congiunte.

#### Stato interno

- `variables`: vettore che contiene le variabili della rete (ordinate secondo gli ID assegnati);
- `adj`: liste di adiacenza che tracciano i figli di ogni variabile (cioè `adj[i]` contiene la lista dei figli della variabile con ID uguale ad i);
- `id`: mappa che permette di trovare l'ID di una variabile dato il suo nome (es. `id['c']` -> 3);
- `topologicalOrder`: contiene gli ID delle variabili ordinate secondo l'ordine topologico.

#### Funzioni per il calcolo delle probabilità

Sono implementate nel file `Network_probability.cpp`.

- `getJointProbability()`: calcola la probabilità congiunta data una configurazione completa di assegnamento;
- `getMarginalProbability()`: calcola la probabilità marginale di una specifica variabile e valore assegnato, per enumerazione completa;
- `getCptRow()`: calcola l'indice della riga della tabella CPT corrispondente a un dato assegnamento.

#### Altre funzioni

- ...

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
    - [ ] **DA MODIFICARE**: attualmente, nel calcolo della probabilità marginale per enumerazione completa sto calcolando tutte le configurazioni complete -> devo invece generare solo le configurazioni delle variabili da cui la variabile target dipende!
4. [x] parsing file BIF
    - [x] deve essere capace di leggere i blocchi variable
    - [x] e di popolare le tabelle CPT leggendo i blocchi probability
    - ~~[ ] TODO: ottimizzare firma della funzione helper **Parser::getCptRow()**, non necessario che sia static, non necessario passaggio per riferimento~~ *rimossa inutile complicazione*
5. [ ] probabilità condizionale
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

### [obsoleto] ~~Fase 2:~~ calcolo della probabilità marginale (enumerazione completa)

- Abbiamo adesso le strutture per rappresentare il network e le CPT.
- Abbiamo una funzione (**getJointProbability**) che dato un assignment completo ci da la probabilità congiunta di quella specifica configuazione (es. *p(a=false, b=true, c=true...)*)
- Adesso dobbiamo creare la funzione che marginalizza la probabilità (es. *p(b=true)*) per **enumerazione completa**, cioè calcolando la probabilità (congiunte) di tutte le configurazioni complete *rilevanti* (cioè, per esempio, in cui *b=true*) e sommandole.
- La probabilità marginale sarà calcolata dalla funzione **getMarginalProbability** che prende in input due stringhe: il nome di una variabile target e il nome del suo valore (es. `getMarginalProbability("b", "true")`).
    - **getMarginalProbability** trova innanzitutto l'ID della variabile target e del valore assegnato
    - crea un array assignment che è inizializzato tutto ad un valore dummy (-1) tranne per l'elemento corrispondente alla variabile target, che inizializzato al valore assegnato (es. true)
    - a questo punto viene chiamata la funzione ricorsiva **marginalRecursive** che prende in input l'ID di una variabile da processare e un assignment completo
- Fatto un test per controllare che le probabilità marginali siano correttamente normalizzate (si veda **Test::marginal()**).

#### [obsoleto] Funzionamento dell'algoritmo ricorsivo di marginalizzazione
- Stato iniziale: viene creato un vettore assignment inizializzato a -1 (ad eccezione della variabile target). La ricorsione parte dalla variabile con id=0.
    - Caso id=n -> il vettore assignment è completo, restituiamo la probabilità congiunta di quella specifica combinazione.
    - Caso assigment[id] != -1 -> quella variabile è l'evidenza oppure il valore è già fissato, saltiamo e chiamiamo ricorsivamente la funzione per la variabile "id+1"
    - Caso assigment[id] == -1 -> ciclo sui possibili valori che la variabile può assumere, sommando tutte le probabilità (per i diversi valori delle altre variabili) chiamando ricorsivamente la funzione.
- **Nota sulla complessità computazionale dell'algoritmo**:
    - *punto di debolezza*: la complessità temporale è esponenziale, per ognuna delle n variabili l'algoritmo deve esplorare K^(n-1) configurazioni -> complessità o(n * K^n) (dove K è il numero di valori possibili)
    - *punto di forza*: basso consumo di memoria, infatti l'algoritmo riutilizza lo stesso vettore di assignment, passato nelle chiamate ricorsive per riferimento: la memoria è occupata unicamente da questo vettore e dalla probabilità, generando una complessità spaziale lineare o(n).

### Fase 2bis: ottimizzazione del calcolo della probabilità marginale
- **PROBLEMA**: l'algoritmo ricorsivo dell'approccio utilizzato fin'ora prova tutte le combinazioni possibili di tutte le variabili della rete, anche quelle che non c'entrano niente con la variabile target. -> **SOLUZIONE**: considerare solo gli antenati (i figli e i rami scollegati non servono nel calcolo, sommando i contributi fanno 1).
    - Esempio: p(b=true) con b che dipende solo da a, lavoro solo su a, invece che su tutte le variabili della rete.
    - Aggiungo un metodo privato **getAncestors(variableId)**, che restituisce l'insieme degli ID antenati di una variabile (genitori, genitori dei genitori, ...), tramite una visita del grafo all'indietro seguendo `Variable::parents`.
    - In **getMarginalProbability**, calcolo l'insieme delle variabili rilevanti (target e tutti i suoi antenati), e filtro `topologicalOrder` mantenendo solo gli ID rilevanti (nell'ordine già corretto).
- Processerò le variabili una alla volta seguendo l'ordine topologico, già implementato nella classe Network.
    - Elimino la ricorsione esplicita (**marginalRecursive**) e la sostituisco con un ciclo.
    - Mantengo una lista di **stati parziali**: coppie (assignment parziale, probabilità accumulata). Parto da un solo stato: target fissato al valore richiesto, probabilità 1.0.
    - Per ognuna delle variabili rilevanti (ordinate secondo ordinamento topologico):
        - se la variabile è già fissata (il target): moltiplico la probabilità di ogni stato per il fattore CPT corrispondente al valore fissato;
        - se la variabile non è ancora fissata: sdoppio ogni stato esistente in tante copie quanti sono i valori possibili della variabile, moltiplicando ciascuna copia per il fattore CPT corrispondente (i genitori sono già assegnati, essendo rispettato l'ordine topologico).
    - Al termine del ciclo, sommo le probabilità di tutti gli stati rimasti: è il risultato della marginale cercata.
    - **Vantaggio**: no ricorsione, no stack di chiamate; il numero di variabili processate è ridotto ai soli antenati rilevanti invece che a tutte le n variabili della rete.
    - Complessità rimane esponenziale nel numero *m* di variabili *rilevanti* -> O(m * K^m)).

### Fase 3: parsing file BIF

- In generale, il modulo Parser deve contenere una funzione del tipo **importBIF(filePath)** che dato il pecorso di un file BIF restituisce l'oggetto di tipo Network costruito secondo i dati trovati nel file.

- Implementerò una classe **Parser** che contiene nel suo stato interno:
    - la stringa **filename** (percorso del file bif importato);
    - l'oggetto di tipo *fstream* di nome **file**;
    - la mappa id (che associa nome -> id) e il vettore variables, che contiene le variabili che andranno a costruire il network;
    - la variabile booleana "**log**" che, quando è settata su true, fa stampare a schermo i log del parsing.
- Il costruttore prenderà in input il nome del file e il valore di log.
- Il metodo **parse()** costruisce e restituisce il Network che si trova al file specificato da filename
    - utilizzerà tre funzioni helper di tipo void che effettueranno il parsing dei 3 blocchi presenti nei files BIF: network, variables, probability;
    - riempirà opportunamente la mappa **id** e l'array **variables**
    - salverà i risultati in un oggetto di tipo Network e lo restituirà in output.
- Il metodo *static* **importBIF()** permetterà di ottenere automaticamente il Network senza dover costruire manualmente l'istanza della classe.

### Fase 4: probabilità condizionale

...

### Fase 5: variable elimination
...