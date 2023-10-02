/**	\file shipbat_official.c
   	\author Simone Ausilio & Francesco Zeno Costanzo
	Per ulteriori dettagli sul programma, consultare il README allegato al messaggio.
	Si dichiara che il contenuto di questo file è in ogni sua parte opera originale degli autori.               */

//(1) PREAMBOLO

#include<stdio.h>
#include<stdlib.h>

//Definiamo alcune costanti macro che useremo nel corso del programma
#define EMPTY 0	//Valore corrispondente ad una cella vuota
#define ONE 1	//Valore corrispondente ad una cella contenente una nave con dimensione di 1 casella
#define TWO 2	//Valore corrispondente ad una cella contenente una parte di nave con dimensione di 2 caselle
#define THREE 3	//Valore corrispondente ad una cella contenente una parte di nave con dimensione di 3 caselle
#define L 10	//Dimensione del campo di battaglia quadrato L*L

//(2) FUNZIONI AUSILIARIE

//Impostiamo una matrice globale che sia accessibile a tutte le funzioni del programma
int sea[L][L];

//(2.1) Determiniamo una funzione che inizializzi la matrice che rappresenta il nostro campo di battaglia con la macro: EMPTY

void init (void) {
	int i, j;
         for (i=0;i<L;i++) {
		for (j=0;j<L; j++) {
			sea[i][j]=EMPTY;
		}
	}
}

//(2.2) Creiamo due funzioni complementari che serviranno per la generazione delle navi da 2 e da 3; i parametri che richiedono entrambe sono, in ordine, le coordinate (rispetto alle righe o alle colonne) della posizione precedentemente generata ('i' o 'j') e la direzione ('direction') originata all'interno delle funzioni; le funzioni restituiscono, rispettivamente, la posizione sulla riga e sulla colonna della parte di nave che ci interessa

int direction_r (int i, int direction) {
	//Con un ciclo 'switch' indichiamo, per ogni caso, le procedure che la funzione deve eseguire
	switch (direction) {
		//Nord
		case 0: if (i==0) return L-1;	//Condizione di bordo continuo della prima riga
			else return i-1; 	//Caso generale
		//Sud
		case 1: if (i==L-1) return 0;	//Condizione di bordo continuo dell'ultima riga
			else return i+1;	//Caso generale

		//Casi in cui la funzione non interviene
		//Est
		case 2: return i;
		//Ovest
		case 3: return i;
	}
}

int direction_c (int j, int direction) {
	//Con un ciclo 'switch' indichiamo, per ogni caso, le procedure che la funzione deve eseguire
	switch (direction) {
		//Casi in cui la funzione non interviene		
		//Nord
		case 0: return j;
		//Sud
		case 1: return j;

		//Est
		case 2: if (j==L-1) return 0;	//Condizione di bordo continuo dell'ultima colonna
			else return j+1;	//Caso generale
		//Ovest
		case 3: if (j==0) return L-1;	//Condizione di bordo continuo della prima colonna
			else return j-1;	//Caso generale
	}
}

//(3) FUNZIONI PER LA GENERAZIONE DELLE NAVI

//(3.1) Impostiamo la prima delle tre funzioni principali, che genera le navi da 1

int put_one (void) {
	//Generazione della coppia di coordinate della nave
	int i=rand()%L;
	int j=rand()%L;

	//Controllo della cella corrispondente alla posizione generata
	if (sea[i][j]==EMPTY) {
		sea[i][j]=ONE;	//Assegnamento del valore 1 alla cella trovata
		return 0;	//Valore restituito nel caso in cui la procedura sia andata a buon fine
	}

	else {
		int p=0; //Inizializzazione della variabile che servirà per la ripetizione del prossimo ciclo 'while'

		//Ciclo 'while' che ha lo scopo di generare una nuova posizione per la nave fino a quando viene trovata una cella libera oppure la variabile 'p' raggiunge il valore di guardia prefissato (che indica il termine della ripetizione)
		while ((p<50) && (sea[i][j]!=EMPTY)) {
			i=rand()%L;
			j=rand()%L;
			p++;
		}
		//Controllo della cella corrispondente alla nuova posizione generata
		if (sea[i][j]==EMPTY) {
                	sea[i][j]=ONE;
			return 0; //Valore restituito nel caso in cui la procedura sia andata a buon fine
		}
	return -1; //Valore restituito nel caso in cui il tentativo sia fallito
	}
}

//(3.2) Definiamo la funzione per la generazione delle navi da 2ù

int put_two (void) {
	//Generazione della prima parte della nave (la prua)
	int i=rand()%L;
	int j=rand()%L;

	/*Generazione della direzione in cui piazzare la seconda parte della nave (la poppa)
	Ricordiamo: 0=Nord; 1=Sud; 2=Est; 3=Ovest*/
	int direction=rand()%4;

	//Assegnamento delle cordinate generate dalle funzioni 'direction_r' e 'direction_c' alle variabili 'f' e 'g'
	int f=direction_r (i, direction);
	int g=direction_c (j, direction);

	//Controllo delle celle corrispondenti alle posizioni generate
	if (sea[i][j]==EMPTY && sea[f][g]==EMPTY) {
		//Assegnamento del valore 2 alle celle trovate
		sea[i][j]=TWO;
		sea[f][g]=TWO;
		return 0; //Valore restituito nel caso in cui la procedura sia andata a buon fine
	}

	else {
		int p=0; //Inizializzazione della variabile che servirà per la ripetizione del prossimo ciclo 'while'

		//Ciclo 'while' che ha lo scopo di generare due nuove posizioni per la nave fino a quando vengono trovate due celle adiacenti libere oppure la variabile 'p' raggiunge il valore di guardia prefissato (che indica il termine della ripetizione)
		while ((p<50) && (sea[i][j]!=EMPTY || sea[f][g]!=EMPTY)) {			
			i=rand()%L;
			j=rand()%L;
			direction=rand()%4;
			f=direction_r (i, direction);
			g=direction_c (j, direction);
			p++;
		}
		//Controllo delle celle corrispondenti alle nuove posizioni generate
		if (sea[i][j]==EMPTY && sea[f][g]==EMPTY) {
			sea[i][j]=TWO;
			sea[f][g]=TWO;
                	return 0; //Valore restituito nel caso in cui la procedura sia andata a buon fine
		}
	return -1; //Valore restituito nel caso in cui il tentativo sia fallito
	}
}

//(3.3) Creiamo l'ultima funzione, atta a immettere nella matrice navi da 3

int put_three (void) {
	//Generazione della prima parte della nave (la prua)
	int i=rand()%L;
	int j=rand()%L;

	//Generazione della direzione in cui piazzare le altre due parti della nave (il centro e la poppa)
	int direction=rand()%4;

	//Generazione della parte centrale della nave
	int f=direction_r(i, direction);
	int g=direction_c(j, direction);

	//Generazione della poppa della nave, fatta in base a quella del centro
	int h=direction_r (f, direction);
	int k=direction_c (g, direction);

	//Controllo delle celle corrispondenti alle posizioni generate
	if (sea[i][j]==EMPTY && sea[f][g]==EMPTY && sea[h][k]==EMPTY) {
		//Assegnamento del valore 3 alle celle trovate
		sea[i][j]=THREE;
		sea[f][g]=THREE;
		sea[h][k]=THREE;
	 	return 0; //Valore restituito nel caso in cui la procedura sia andata a buon fine
	}

	else {	
		int p = 0; //Inizializzazione della variabile che servirà per la ripetizione del prossimo ciclo 'while'
		//Ciclo 'while' che ha lo scopo di generare tre nuove posizioni per la nave fino a quando vengono trovate tre celle adiacenti libere oppure la variabile 'p' raggiunge il valore di guardia prefissato (che indica il termine della ripetizione)
		while ((p <50) && (sea[i][j]!=EMPTY || sea[f][g]!=EMPTY || sea[h][k]!=EMPTY)) {
			i=rand()%L;
			j=rand()%L;
			direction=rand()%4;
			f=direction_r (i, direction);
			g=direction_c (j, direction);
			h=direction_r (f, direction);
			k=direction_c (g, direction);
			p++;
		}
		//Controllo delle celle corrispondenti alle nuove posizioni generate
		if (sea[i][j] == EMPTY && sea[f][g]==EMPTY && sea[h][k]==EMPTY) {
			sea[i][j]=THREE; 
			sea[f][g]=THREE;
			sea[h][k]=THREE;
			return 0; //Valore restituito nel caso in cui la procedura sia andata a buon fine
		}
	return -1; //Valore restituito nel caso in cui il tentativo sia fallito
	}
}
shipbat_official.c
