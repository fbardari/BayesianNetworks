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