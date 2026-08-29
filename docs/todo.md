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
    - [x] *modificato*: attualmente, nel calcolo della probabilità marginale per enumerazione completa sto calcolando tutte le configurazioni complete -> devo invece generare solo le configurazioni delle variabili da cui la variabile target dipende!
4. [x] parsing file BIF
    - [x] deve essere capace di leggere i blocchi variable
    - [x] e di popolare le tabelle CPT leggendo i blocchi probability
    - ~~[ ] TODO: migliorare firma della funzione helper **Parser::getCptRow()**, non necessario che sia static, non necessario passaggio per riferimento~~ *rimossa inutile complicazione*
    - [x] *fatto*: rimuovere `file >> s` per lettura dei token, scrivere una funzione `nextToken()` che separi correttamente i token non spaziati
5. [x] eventuale interfaccia utente
6. [x] probabilità condizionale (implementata nella query principale di VE)
7. [x] variable elimination
