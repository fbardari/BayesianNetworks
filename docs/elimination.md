## Variable Elimination

- [Riferimento principale](https://www.youtube.com/playlist?list=PLdBx38JxhMNsJ4QcZ7OaIaSE1HBYNs-7u)

### Definizione dei fattori

- Un fattore $F(X_1, ..., X_n)$ è una funzione che associa: ad ogni configurazione di un set di variabili ${X_1, ..., X_n}$ -> un numero.
- Saranno utilizzati rappresentare probabilità congiunte o condizionali.

Nell'header Factor.hpp definirò la classe Factor con tutte le sue operazioni.

### Operazioni sui fattori

#### Restrict

- Assegna un valore ad una variabile.
- **Input**: fattore $F(X_1, X_2, ..., X_n)$, $X_1 = v_1$
- **Output**: nuovo fattore $F(X_1=v_1, X_2, ..., X_n)$ con set di variabili $(X_2, ..., X_n)$

#### Sum out

Elimina una variabile sommando su tutti i possibili valori di quella variabile.

$$
\sum_{X_1} F(X_1, X_2, ..., X_n)
$$

#### Prodotto

- Moltiplica due fattori.
- Il prodotto di due fattori $F_1(X,Y)$ e $F_2(Y,Z)$ dove Y è una variabile che hanno in comune è definito come

$$
(F_1 \times F_2) (X,Y,Z) = F_1(X,Y) * F_2(Y,Z)
$$

#### Normalizzazione

- Operazione che converte un fattore in una distribuzione di probabilità (ultimo step della variable elimination).
- Ogni valore è diviso per la somma di tutti i valori.

### Algoritmo di Variable Elimination

Per ogni variabile del network, la CPT viene convertita in un Factor:
- scope = variabile + i suoi genitori;
- table = valori della CPT.

Se stiamo calcolando la probabilità condizionale: P(X | E1=e1, E2=e2, ...), per ogni variabile di evidenza Ei e per ogni fattore che la contiene, facciamo:

F = F.restrict(Ei, ei)

Successivamente, eliminiamo tutte le variabili non target e non di evidenza (seguendo un ordine prefissato): per ciascuna variabile, moltiplichiamo tra di loro i fattori che la contengono e poi applichiamo il sumOut().

Alla fine, moltiplichiamo tutti i fattori rimasti (che contengono solo la variabile target X), normalizziamo il fattore risultante con normalize() e possiamo leggere le probabilità dal fattore ottenuto.