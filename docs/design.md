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

- [ ] parsing file BIF
- [ ] costruzione del DAG
- [ ] ordinamento topologico
- [ ] calcolo probabilità congiunta
- [ ] probabilità marginale (con enumerazione completa)
- [ ] variable elimination