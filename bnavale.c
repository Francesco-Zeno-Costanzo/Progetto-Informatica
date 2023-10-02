/*	\file bnavale.c
   	\author Simone Ausilio & Francesco Zeno Costanzo.
	\Si dichiara che il contenuto di questo file è in ogni sua parte opera originale degli autori.	*/

//(1)PREAMBOLO

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Definizione di alcune variabili globali. */
#define EMPTY  				'.'
#define ONE    				'1'
#define TWO    				'2'
#define THREE  				'3'

#define EMPTY_COLPITA  		'X'
#define ONE_COLPITA    		'a'
#define TWO_COLPITA    		'b'
#define THREE_COLPITA  		'c'

#define ERRORE           	130
#define ACQUA            	131
#define AFFONDATA_ONE    	132
#define AFFONDATA_TWO    	133
#define AFFONDATA_THREE  	134
#define COLPITA_TWO      	135
#define COLPITA_THREE    	136

//(2)FUNZIONI AUSILIARIE

/* (2.1)Definizione del tipo utilizzato per il campo da gioco. */
typedef struct area {
	//Matrice che rappresenta l'area di gioco
	char **a;
	//Numero di righe
	unsigned N;
	//Numero di colonne
	unsigned M;
	//Numero di navi presenti
	unsigned n_navi;
} area_t;

/* (2.2)Funzione che inizializza a EMPTY la matrice del campo da gioco. Parametri: campo da gioco. Ritorno: void. */
void init (area_t *sea) {
	int i, j;
	for (i = 0; i < sea->N; i++) {
		for (j = 0; j < sea->M; j++) {
			sea->a[i][j] = EMPTY;
		}
	}
}

/* (2.3)Funzione che alloca l'area di gioco di lunghezza nxm. Parametri: numero di righe, numero di colonne. Ritorno: campo da gioco correttamente inizializzato. */
area_t *crea_area (unsigned n, unsigned m) {
	int i;
	area_t *sea;
	//Allocazione della struttura
	sea = malloc (sizeof(area_t));
	if (sea == NULL) return NULL;
	//Assegnamento delle dimensioni della matrice
	sea->N = n;
	sea->M = m;
	//Allocazione delle righe
	sea->a = malloc (sea->N * sizeof(char *));
	if (sea->a == NULL) return NULL;
	//Allocazione delle colonne per ciascuna riga
	for (i = 0; i < n; i++) {
		sea->a[i] = malloc (sea->M * sizeof(char));
		if (sea->a[i] == NULL) return NULL;
	}
	//Inizializzazione del numero iniziale di navi a 0
	sea->n_navi = 0;
	//Impostazione a EMPTY dell'area di gioco
	init (sea);
	return sea;
}

/* (2.4)Funzione che libera la memoria allocata dalla funzione (2.3). Parametri: campo da gioco. Ritorno: void. */
void distruggi_area (area_t *sea) {
	int i;
	//Deallocamento delle colonne
	for (i = 0; i < sea->N; i++) {
		free (sea->a[i]);
	}
	//Deallocamento delle righe
	free (sea->a);
	//Deallocamento della variabile
	free (sea);
}

/* (2.5)Funzione di controllo dell'area (quadrata) che circonda la casella generata casualmente. Parametri: posizione sulle righe, posizione sulle colonne, area di gioco. Ritorno: 0 se l'area è libera, -1 se almeno una delle caselle appartenenti all'area è occupata. */
int area_check (int a, int b, area_t *sea) {
	int i, j;

	/*(2.5.1)Condizioni di contorno generalizzate*/

	//(2.5.1.1)Controllo di bordo della prima riga generale
	if (a == 0 && b != 0 && b != sea->M-1) {
		//Controllo dell'ultima riga
		i = sea->N-1;
		for (j = b-1; j <= b+1; j++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		//Controllo della prima e della seconda riga
		for (i = 0; i <= 1; i++) {
			for (j = b-1; j <= b+1; j++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		return 0;
	}
	//(2.5.1.2)Controllo di bordo dell'ultima riga generale
	else if (a == sea->N-1 && b != 0 && b != sea->M-1) {
		//Controllo della prima riga
		i = 0;
		for (j = b-1; j <= b+1; j++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		//Controllo della penultima e dell'ultima riga
		for (i = a-1; i <= a; i++) {
			for (j = b-1; j <= b+1; j++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		return 0;
	}
	//(2.5.1.3)Controllo di bordo dell'ultima colonna generale
	else if (b == sea->M-1 && a != 0 && a != sea->N-1) {
		//Controllo della prima colonna
		j = 0;
		for (i = a-1; i <= a+1; i++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		//Controllo della penultima e dell'ultima colonna
		for (j = b-1; j <= b; j++) {
			for (i = a-1; i <= a+1; i++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		return 0;
	}
	//(2.5.1.4)Controllo di bordo della prima colonna generale
	else if (b == 0 && a != 0 && a != sea->N-1) {
		//Controllo dell'ultima colonna
		j = sea->M-1;
		for (i = a-1; i <= a+1; i++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		//Controllo della prima e della seconda colonna
		for (j = 0; j <= 1; j++) {
			for (i = a-1; i <= a+1; i++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		return 0;
	}

	/*(2.5.2)Condizioni di contorno ai vertici*/

	//(2.5.2.1)Controllo della prima casella in alto a sinistra
	else if (a == 0 && b == 0) {
		//Controllo delle caselle naturalmente adiacenti
		for (i = a; i <= a+1; i++) {
			for (j = b; j <= b+1; j++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		//Controllo della casella all'estremo opposto
		if (sea->a[sea->N-1][sea->M-1] != EMPTY) return -1;
		//Controllo dell'ultima colonna
		j = sea->M-1;
		for (i = a; i <= a+1; i++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		//Controllo dell'ultima riga
		i = sea->N-1;
		for (j = b; j <= b+1; j++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		return 0;
	}
	//(2.5.2.2)Controllo della prima casella in alto a destra
	else if (a == 0 && b == sea->M-1) {
		//Controllo delle caselle naturalmente adiacenti
		for (i = a; i <= a+1; i++) {
			for (j = b-1; j <= b; j++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		//Controllo della casella all'estremo opposto
		if (sea->a[sea->N-1][0] != EMPTY) return -1;
		//Controllo della prima colonna
		j = 0;
		for (i = a; i <= a+1; i++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		//Controllo dell'ultima riga
		i = sea->N-1;
		for (j = b-1; j <= b; j++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		return 0;
	}
	//(2.5.2.3)Controllo della prima casella in basso a destra
	else if (a == sea->N-1 && b == sea->M-1) {
		//Controllo delle caselle naturalmente adiacenti
		for (i = a-1; i <= a; i++) {
			for (j = b-1; j <= b; j++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		//Controllo della casella all'estremo opposto
		if (sea->a[0][0] != EMPTY) return -1;
		//Controllo della prima colonna
		j = 0;
		for (i = a-1; i <= a; i++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		//Controllo della prima riga
		i = 0;
		for (j = b-1; j <= b; j++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		return 0;
	}
	//(2.5.2.4)Controllo della prima casella in basso a sinistra
	else if (a == sea->N-1 && b == 0) {
		//Controllo delle caselle naturalmente adiacenti
		for (i = a-1; i <= a; i++) {
			for (j = b; j <= b+1; j++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		//Controllo della casella all'estremo opposto
		if (sea->a[0][sea->M-1] != EMPTY) return -1;
		//Controllo dell'ultima colonna
		j = sea->M-1;
		for (i = a-1; i <= a; i++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		//Controllo della prima riga
		i = 0;
		for (j = b; j <= b+1; j++) {
			if (sea->a[i][j] != EMPTY) return -1;
		}
		return 0;
	}

	/*(2.5.3)Condizione generale*/

	else {
		for (i = a-1; i <= a+1; i++) {
			for (j= b-1; j <= b+1; j++) {
				if (sea->a[i][j] != EMPTY) return -1;
			}
		}
		return 0;
	}
}

/* (2.6)Funzione che genera la posizione di un pezzo di nave da 2 o da 3 a partire dalla parte precedentemente generata. Parametri: posizione sulle righe della parte iniziale, puntatore alla posizione sulle colonne del pezzo di partenza, direzione di generazione (un numero casuale tra 0 e 3), campo da gioco. Ritorno: posizione sulle righe della nuova parte di nave. */
int direction (int a, int *b, int direction, area_t *sea) {
	switch (direction) {
		//Nord
		case 0: if (a == 0) {
				//Condizione di bordo della prima riga
				a = sea->N-1;
				break;
			}
			else {
				//Caso generale
				a--;
				break;
			}
		//Sud
		case 1: if (a == sea->N-1) {
				//Condizione di bordo dell'ultima riga
				a = 0;
				break;
			}
			else {
				//Caso generale
				a++;
				break;
			}
		//Est
		case 2: if (*b == sea->M-1) {
				//Condizione di bordo dell'ultima colonna
				*b = 0;
				break;
			}
			else {
				//Caso generale
				*b = *b + 1;
				break;
			}
		//Ovest
		case 3: if (*b == 0) {
				//Condizione di bordo della prima colonna
				*b = sea->M-1;
				break;
			}
			else {
				//Caso generale
				*b = *b - 1;
				break;
			}
	}
	return a;
}

//(3)FUNZIONI DI GENERAZIONE DELLE NAVI

/* (3.1)Funzione per la generazione delle navi di dimensione 1. Parametri: area di gioco. Ritorno: 0 se il processo è andato a buon fine, -1 altrimenti. */
int put_one (area_t *sea) {
	//Generazione della prima coppia di coordinate
	int prua_r_1 = rand()%sea->N;
	int prua_c_1 = rand()%sea->M;
	//Controllo della posizione
	if (area_check (prua_r_1, prua_c_1, sea) == 0) {
		sea->a[prua_r_1][prua_c_1] = ONE;
		sea->n_navi++;
		return 0;
	}
	else {
		//Definizione del contatore e del tetto di ripetizioni
		int k = 0, P = sea->N * sea->M;
		//Ciclo di generazione della posizione
		while (k < P && area_check (prua_r_1, prua_c_1, sea) != 0) {
			prua_r_1 = rand()%sea->N;
			prua_c_1 = rand()%sea->M;
			k++;
		}
		//Controllo della posizione
		if (area_check (prua_r_1, prua_c_1, sea) == 0) {
			sea->a[prua_r_1][prua_c_1] = ONE;
			sea->n_navi++;
			return 0;
		}
		else return -1;
	}
}

/* (3.2)Funzione per la generazione delle navi di dimensione 2 (stessi parametri e ritorni di prima). */
int put_two (area_t *sea) {
	//Generazione della prima coppia di coordinate per il primo pezzo di nave
	int prua_r_2 = rand()%sea->N;
	int prua_c_2 = rand()%sea->M;
	//Generazione della direzione e della prima coppia di coordinate del pezzo rimanente
	int dir = rand()%4;
	int poppa_r_2 = prua_r_2;
	int poppa_c_2 = prua_c_2;
	poppa_r_2 = direction (poppa_r_2, &poppa_c_2, dir, sea);
	//Controllo delle posizioni
	if ((area_check (prua_r_2, prua_c_2, sea) == 0) && (area_check (poppa_r_2, poppa_c_2, sea) == 0)) {
		sea->a[prua_r_2][prua_c_2] = TWO;
		sea->a[poppa_r_2][poppa_c_2] = TWO;
		sea->n_navi++;
		return 0;
	}
	else {
		//Definizione del contatore e del tetto di ripetizioni
		int k = 0, P = sea->N * sea->M;
		//Ciclo di generazione delle posizioni
		while (k < P && (area_check (prua_r_2, prua_c_2, sea) != 0 || area_check (poppa_r_2, poppa_c_2, sea) != 0)) {
			prua_r_2 = rand()%sea->N;
 			prua_c_2 = rand()%sea->M;
			dir = rand()%4;
			poppa_r_2 = prua_r_2;
			poppa_c_2 = prua_c_2;
			poppa_r_2 = direction (poppa_r_2, &poppa_c_2, dir, sea);
			k++;
		}
		//Controllo delle posizioni
		if ((area_check (prua_r_2, prua_c_2, sea) == 0) && (area_check (poppa_r_2, poppa_c_2, sea) == 0)) {
			sea->a[prua_r_2][prua_c_2] = TWO;
			sea->a[poppa_r_2][poppa_c_2] = TWO;
			sea->n_navi++;
			return 0;
		}
		else return -1;
	}
}

/* (3.3)Funzione per la generazione delle navi di dimensione 3 (stessi parametri e ritorni di prima). */
int put_three (area_t *sea) {
	//Generazione della prima coppia di coordinate per il primo pezzo di nave
	int prua_r_3 = rand()%sea->N;
	int prua_c_3 = rand()%sea->M;
	//Generazione della direzione e della prima coppia di coordinate per la seconda parte di nave
	int dir = rand()%4;
	int centro_r_3 = prua_r_3;
	int centro_c_3 = prua_c_3;
	centro_r_3 = direction (centro_r_3, &centro_c_3, dir, sea);
	//Generazione del pezzo rimanente
	int poppa_r_3 = centro_r_3;
	int poppa_c_3 = centro_c_3;
	poppa_r_3 = direction (poppa_r_3, &poppa_c_3, dir, sea);
	//Controllo delle posizioni
	if ((area_check (prua_r_3, prua_c_3, sea) == 0) && (area_check (centro_r_3, centro_c_3, sea) == 0) && (area_check (poppa_r_3, poppa_c_3, sea) == 0)) {
		sea->a[prua_r_3][prua_c_3] = THREE;
		sea->a[centro_r_3][centro_c_3] = THREE;
		sea->a[poppa_r_3][poppa_c_3] = THREE;
		sea->n_navi++;
		return 0;
	}
	else {
		//Definizione del contatore e del tetto di ripetizioni
		int k = 0, P = sea->N * sea->M;
		//Ciclo di generazione delle posizioni
		while (k < P && (area_check (prua_r_3, prua_c_3, sea) != 0 || area_check (centro_r_3, centro_c_3, sea) != 0 || area_check (poppa_r_3, poppa_c_3, sea) != 0)) {
				prua_r_3 = rand()%sea->N;
				prua_c_3 = rand()%sea->M;
				dir = rand()%4;
				centro_r_3 = prua_r_3;
				centro_c_3 = prua_c_3;
				centro_r_3 = direction (centro_r_3, &centro_c_3, dir, sea);
				poppa_r_3 = centro_r_3;
				poppa_c_3 = centro_c_3;
				poppa_r_3 = direction (poppa_r_3, &poppa_c_3, dir, sea);
				k++;
		}
		//Controllo delle posizioni
		if ((area_check (prua_r_3, prua_c_3, sea) == 0) && (area_check (centro_r_3, centro_c_3, sea) == 0) && (area_check (poppa_r_3, poppa_c_3, sea) == 0)) {
			sea->a[prua_r_3][prua_c_3] = THREE;
			sea->a[centro_r_3][centro_c_3] = THREE;
			sea->a[poppa_r_3][poppa_c_3] = THREE;
			sea->n_navi++;
			return 0;
		}
		else return -1;
	}
}

//(4)FUNZIONI AUSILIARIE AL GIOCO DELLA BATTAGLIA NAVALE

/* (4.1)Funzione (specifica per le navi da 2) che controlla la presenza di pezzi di nave da 2 affondati nelle caselle adiacenti a quella colpita da una mossa (non sono interessate le celle in diagonale). Parametri: campo da gioco, coordinate sulla matrice del colpo sparato. Ritorno: 0 se c'è, -1 altrimenti. */
int sunk_two (area_t *sea, int a, int b) {

	/*(4.1.1)Condizioni di contorno generalizzate*/

	//(4.1.1.1)Controllo di bordo della prima riga generale
	if (a == 0 && b != 0 && b != sea->M-1) {
		if (sea->a[sea->N-1][b] == TWO_COLPITA || sea->a[a+1][b] == TWO_COLPITA || sea->a[a][b+1] == TWO_COLPITA || sea->a[a][b-1] == TWO_COLPITA) return 0;
		else return -1;
	}
	//(4.1.1.2)Controllo di bordo dell'ultima riga generale
	else if (a == sea->N-1 && b != 0 && b != sea->M-1) {
		if (sea->a[a-1][b] == TWO_COLPITA || sea->a[0][b] == TWO_COLPITA || sea->a[a][b+1] == TWO_COLPITA || sea->a[a][b-1] == TWO_COLPITA) return 0;
		else return -1;
	}
	//(4.1.1.3)Controllo di bordo dell'ultima colonna generale
	else if (b == sea->M-1 && a != 0 && a != sea->N-1) {
		if (sea->a[a-1][b] == TWO_COLPITA || sea->a[a+1][b] == TWO_COLPITA || sea->a[a][0] == TWO_COLPITA || sea->a[a][b-1] == TWO_COLPITA) return 0;
		else return -1;
	}
	//(4.1.1.4)Controllo di bordo della prima colonna generale
	else if (b == 0 && a != 0 && a != sea->N-1) {
		if (sea->a[a-1][b] == TWO_COLPITA || sea->a[a+1][b] == TWO_COLPITA || sea->a[a][b+1] == TWO_COLPITA || sea->a[a][sea->M-1] == TWO_COLPITA) return 0;
		else return -1;
	}

	/*(4.1.2)Condizioni di contorno ai vertici*/

	//(4.1.2.1)Controllo della prima casella in alto a sinistra
	else if (a == 0 && b == 0) {
		if (sea->a[sea->N-1][b] == TWO_COLPITA || sea->a[a+1][b] == TWO_COLPITA || sea->a[a][b+1] == TWO_COLPITA || sea->a[a][sea->M-1] == TWO_COLPITA) return 0;
		else return -1;
	}
	//(4.1.2.2)Controllo della prima casella in alto a destra
	else if (a == 0 && b == sea->M-1) {
		if (sea->a[sea->N-1][b] == TWO_COLPITA || sea->a[a+1][b] == TWO_COLPITA || sea->a[a][0] == TWO_COLPITA || sea->a[a][b-1] == TWO_COLPITA) return 0;
		else return -1;
	}
	//(4.1.2.3)Controllo della prima casella in basso a destra
	else if (a == sea->N-1 && b == sea->M-1) {
		if (sea->a[a-1][b] == TWO_COLPITA || sea->a[0][b] == TWO_COLPITA || sea->a[a][0] == TWO_COLPITA || sea->a[a][b-1] == TWO_COLPITA) return 0;
		else return -1;
	}
	//(4.1.2.4)Controllo della prima casella in basso a sinistra
	else if (a == sea->N-1 && b == 0) {
		if (sea->a[a-1][b] == TWO_COLPITA || sea->a[0][b] == TWO_COLPITA || sea->a[a][b+1] == TWO_COLPITA || sea->a[a][sea->M-1] == TWO_COLPITA) return 0;
		else return -1;
	}

	/*(4.1.3)Condizione generale*/

	else {
		if (sea->a[a-1][b] == TWO_COLPITA || sea->a[a+1][b] == TWO_COLPITA || sea->a[a][b+1] == TWO_COLPITA || sea->a[a][b-1] == TWO_COLPITA) return 0;
		else return -1;
	}
}

/* (4.2)Funzione (specifica per le navi da 3, per la loro parte centrale) che controlla la presenza di pezzi di nave da 3 affondati nelle caselle adiacenti a quella colpita da una mossa (non sono interessate le celle in diagonale). Parametri: campo da gioco, coordinate sulla matrice del colpo sparato. Ritorno: 0 se c'è, -1 altrimenti. */
int sunk_center (area_t *sea, int a, int b) {

	/*(4.2.1)Condizioni di contorno generalizzate*/

	//(4.2.1.1)Controllo di bordo della prima riga generale
	if (a == 0 && b != 0 && b != sea->M-1) {
		if ((sea->a[sea->N-1][b] == THREE_COLPITA && sea->a[a+1][b] == THREE_COLPITA) || (sea->a[a][b+1] == THREE_COLPITA && sea->a[a][b-1] == THREE_COLPITA)) return 0;
		else return -1;
	}
	//(4.2.1.2)Controllo di bordo dell'ultima riga generale
	else if (a == sea->N-1 && b != 0 && b != sea->M-1) {
		if ((sea->a[a-1][b] == THREE_COLPITA && sea->a[0][b] == THREE_COLPITA) || (sea->a[a][b+1] == THREE_COLPITA && sea->a[a][b-1] == THREE_COLPITA)) return 0;
		else return -1;
	}
	//(4.2.1.3)Controllo di bordo dell'ultima colonna generale
	else if (b == sea->M-1 && a != 0 && a != sea->N-1) {
		if ((sea->a[a-1][b] == THREE_COLPITA && sea->a[a+1][b] == THREE_COLPITA) || (sea->a[a][0] == THREE_COLPITA && sea->a[a][b-1] == THREE_COLPITA)) return 0;
		else return -1;
	}
	//(4.2.1.4)Controllo di bordo della prima colonna generale
	else if (b == 0 && a != 0 && a != sea->N-1) {
		if ((sea->a[a-1][b] == THREE_COLPITA && sea->a[a+1][b] == THREE_COLPITA) || (sea->a[a][b+1] == THREE_COLPITA && sea->a[a][sea->M-1] == THREE_COLPITA)) return 0;
		else return -1;
	}

	/*(4.2.2)Condizioni di contorno ai vertici*/

	//(4.2.2.1)Controllo della prima casella in alto a sinistra
	else if (a == 0 && b == 0) {
		if ((sea->a[sea->N-1][b] == THREE_COLPITA && sea->a[a+1][b] == THREE_COLPITA) || (sea->a[a][b+1] == THREE_COLPITA && sea->a[a][sea->M-1] == THREE_COLPITA)) return 0;
		else return -1;
	}
	//(4.2.2.2)Controllo della prima casella in alto a destra
	else if (a == 0 && b == sea->M-1) {
		if ((sea->a[sea->N-1][b] == THREE_COLPITA && sea->a[a+1][b] == THREE_COLPITA) || (sea->a[a][0] == THREE_COLPITA && sea->a[a][b-1] == THREE_COLPITA)) return 0;
		else return -1;
	}
	//(4.2.2.3)Controllo della prima casella in basso a destra
	else if (a == sea->N-1 && b == sea->M-1) {
		if ((sea->a[a-1][b] == THREE_COLPITA && sea->a[0][b] == THREE_COLPITA) || (sea->a[a][0] == THREE_COLPITA && sea->a[a][b-1] == THREE_COLPITA)) return 0;
		else return -1;
	}
	//(4.2.2.4)Controllo della prima casella in basso a sinistra
	else if (a == sea->N-1 && b == 0) {
		if ((sea->a[a-1][b] == THREE_COLPITA && sea->a[0][b] == THREE_COLPITA) || (sea->a[a][b+1] == THREE_COLPITA && sea->a[a][sea->M-1] == THREE_COLPITA)) return 0;
		else return -1;
	}

	/*(4.2.3)Condizione generale*/

	else {
		if ((sea->a[a-1][b] == THREE_COLPITA && sea->a[a+1][b] == THREE_COLPITA) || (sea->a[a][b+1] == THREE_COLPITA && sea->a[a][b-1] == THREE_COLPITA)) return 0;
		else return -1;
	}
}

/* (4.3)Funzione (specifica per le navi da 3, per una delle due estremità) che controlla la presenza di pezzi di nave da 3 affondati nelle caselle adiacenti a quella colpita da una mossa (non sono interessate le celle in diagonale). Parametri: campo da gioco, coordinate sulla matrice del colpo sparato. Ritorno: funzione sunk_center valutata sulla posizione del centro colpito, -1 se non è stato trovato nulla. */
int sunk_three (area_t *sea, int a, int b) {

	/*(4.3.1)Condizioni di contorno generalizzate*/

	//(4.3.1.1)Controllo di bordo della prima riga generale
	if (a == 0 && b != 0 && b != sea->M-1) {
		if (sea->a[sea->N-1][b] == THREE_COLPITA) return sunk_center (sea, sea->N-1, b);
		else if (sea->a[a+1][b] == THREE_COLPITA) return sunk_center (sea, a+1, b);
		else if (sea->a[a][b+1] == THREE_COLPITA) return sunk_center (sea, a, b+1);
		else if (sea->a[a][b-1] == THREE_COLPITA) return sunk_center (sea, a, b-1);
		else return -1;
	}
	//(4.3.1.2)Controllo di bordo dell'ultima riga generale
	else if (a == sea->N-1 && b != 0 && b != sea->M-1) {
		if (sea->a[a-1][b] == THREE_COLPITA) return sunk_center (sea, a-1, b);
		else if (sea->a[0][b] == THREE_COLPITA) return sunk_center (sea, 0, b);
		else if (sea->a[a][b+1] == THREE_COLPITA) return sunk_center (sea, a, b+1);
		else if (sea->a[a][b-1] == THREE_COLPITA) return sunk_center (sea, a, b-1);
		else return -1;
	}
	//(4.3.1.3)Controllo di bordo dell'ultima colonna generale
	else if (b == sea->M-1 && a != 0 && a != sea->N-1) {
		if (sea->a[a-1][b] == THREE_COLPITA) return sunk_center (sea, a-1, b);
		else if (sea->a[a+1][b] == THREE_COLPITA) return sunk_center (sea, a+1, b);
		else if (sea->a[a][0] == THREE_COLPITA) return sunk_center (sea, a, 0);
		else if (sea->a[a][b-1] == THREE_COLPITA) return sunk_center (sea, a, b-1);
		else return -1;
	}
	//(4.3.1.4)Controllo di bordo della prima colonna generale
	else if (b == 0 && a != 0 && a != sea->N-1) {
		if (sea->a[a-1][b] == THREE_COLPITA) return sunk_center (sea, a-1, b);
		else if (sea->a[a+1][b] == THREE_COLPITA) return sunk_center (sea, a+1, b);
		else if (sea->a[a][b+1] == THREE_COLPITA) return sunk_center (sea, a, b+1);
		else if (sea->a[a][sea->M-1] == THREE_COLPITA) return sunk_center (sea, a, sea->M-1);
		else return -1;
	}

	/*(4.3.2)Condizioni di contorno ai vertici*/

	//(4.3.2.1)Controllo della prima casella in alto a sinistra
	else if (a == 0 && b == 0) {
		if (sea->a[sea->N-1][b] == THREE_COLPITA) return sunk_center (sea, sea->N-1, b);
		else if (sea->a[a+1][b] == THREE_COLPITA) return sunk_center (sea, a+1, b);
		else if (sea->a[a][b+1] == THREE_COLPITA) return sunk_center (sea, a, b+1);
		else if (sea->a[a][sea->M-1] == THREE_COLPITA) return sunk_center (sea, a, sea->M-1);
		else return -1;
	}
	//(4.3.2.2)Controllo della prima casella in alto a destra
	else if (a == 0 && b == sea->M-1) {
		if (sea->a[sea->N-1][b] == THREE_COLPITA) return sunk_center (sea, sea->N-1, b);
		else if (sea->a[a+1][b] == THREE_COLPITA) return sunk_center (sea, a+1, b);
		else if (sea->a[a][0] == THREE_COLPITA) return sunk_center (sea, a, 0);
		else if (sea->a[a][b-1] == THREE_COLPITA) return sunk_center (sea, a, b-1);
		else return -1;
	}
	//(4.3.2.3)Controllo della prima casella in basso a destra
	else if (a == sea->N-1 && b == sea->M-1) {
		if (sea->a[a-1][b] == THREE_COLPITA) return sunk_center (sea, a-1, b);
		else if (sea->a[0][b] == THREE_COLPITA) return sunk_center (sea, 0, b);
		else if (sea->a[a][0] == THREE_COLPITA) return sunk_center (sea, a, 0);
		else if (sea->a[a][b-1] == THREE_COLPITA) return sunk_center (sea, a, b-1);
		else return -1;
	}
	//(4.3.2.4)Controllo della prima casella in basso a sinistra
	else if (a == sea->N-1 && b == 0) {
		if (sea->a[a-1][b] == THREE_COLPITA) return sunk_center (sea, a-1, b);
		else if (sea->a[0][b] == THREE_COLPITA) return sunk_center (sea, 0, b);
		else if (sea->a[a][b+1] == THREE_COLPITA) return sunk_center (sea, a, b+1);
		else if (sea->a[a][sea->M-1] == THREE_COLPITA) return sunk_center (sea, a, sea->M-1);
		else return -1;
	}

	/*(4.3.3)Condizione generale*/

	else {
		if (sea->a[a-1][b] == THREE_COLPITA) return sunk_center (sea, a-1, b);
		else if (sea->a[a+1][b] == THREE_COLPITA) return sunk_center (sea, a+1, b);
		else if (sea->a[a][b+1] == THREE_COLPITA) return sunk_center (sea, a, b+1);
		else if (sea->a[a][b-1] == THREE_COLPITA) return sunk_center (sea, a, b-1);
		else return -1;
	}
}

/* (4.4)Funzione che controlla l'esito della mossa effettuata dal partecipante e che modifica il campo da gioco. Parametri: area di gioco, coordinate sulla matrice del colpo sparato. Ritorno: valore dell'esito della mossa (definiti da variabili macro di tipo intero). */
int esegui_mossa (area_t *sea, int a, int b) {
	//Colpo finito in acqua
	if (sea->a[a][b] == EMPTY) {
		sea->a[a][b] = EMPTY_COLPITA;
		return ACQUA;
	}
	//Colpo finito su una nave da 1
	else if (sea->a[a][b] == ONE) {
		sea->a[a][b] = ONE_COLPITA;
		sea->n_navi--;
		return AFFONDATA_ONE;
	}
	//Colpo finito su una nave da 2
	else if (sea->a[a][b] == TWO) {
		sea->a[a][b] = TWO_COLPITA;
		//Controllo dell'altro pezzo di nave
		if (sunk_two (sea, a, b) == 0) {
			sea->n_navi--;
			return AFFONDATA_TWO;
		}	
		else return COLPITA_TWO;
	}
	//Colpo finito su una nave da 3
	else if (sea->a[a][b] == THREE) {
		sea->a[a][b] = THREE_COLPITA;
		//Controllo degli altri pezzi di nave
		if (sunk_three (sea, a, b) == 0 || sunk_center (sea, a, b) == 0) {
			sea->n_navi--;
			return AFFONDATA_THREE;
		}
		else return COLPITA_THREE;
	}
	//Gestione dell'errore (quando ad esempio si attacca una casella già colpita)
	else return ERRORE;
}

//(5)GIOCO DELLA BATTAGLIA NAVALE

/* Dichiarazione di alcune funzioni usate in seguito. */
void print_area (area_t *sea);
void stampa_esito (int e);

/* (5.1)Funzione che stampa la matrice del computer nascondendo le navi e rappresentando solo i colpi sparati. Parametri: campo da gioco. Ritorno: void. */
void print_CPU (area_t *sea){
	int i, j;
	//Gestione dell'errore
	if (sea == NULL) return;
	//Stampa della matrice "coperta"  
	for (i = 0; i < sea->N; i++) {
		for (j = 0; j < sea->M; j++) {
			if (sea->a[i][j] == '1' || sea->a[i][j] == '2' || sea->a[i][j] == '3') printf (". ");
			else printf ("%c ", sea->a[i][j]);
   		}
   	printf ("\n");
	}  
	putchar ('\n');
	printf ("#navi: %d", sea->n_navi);
	putchar ('\n');
	putchar ('\n');
	return; 
}

/* (5.2)Funzione addetta a controllare il flusso del gioco. Parametri: numero di righe dell'area di gioco, numero di colonne, numero di navi da 1, numero di navi da 2, numero di navi da 3. Ritorno: void. */
void gioca (unsigned n, unsigned m, unsigned n_navi1, unsigned n_navi2, unsigned n_navi3) {
	int a, b;
	int i, N = 1;
	//Area di gioco del computer
	area_t *sea_CPU;
	//Area di gioco del giocatore
	area_t *sea_HUMAN;
	//Creazione e inizializzazione del campo da gioco del computer
	sea_CPU = crea_area (n, m);
 	init (sea_CPU);
	//Posizionamento delle navi sull'area appartenente al computer
	for (i = 0; i < n_navi1; i++) {
		put_one (sea_CPU);
	}
	for (i = 0; i < n_navi2; i++) {
		put_two (sea_CPU);
	}
	for (i = 0; i < n_navi3; i++) {
		put_three (sea_CPU);
	}
	//Creazione e inizializzazione del campo di gioco del giocatore
	sea_HUMAN = crea_area (n, m);
 	init (sea_HUMAN);
	//Posizionamento delle navi sull'area appartenente al giocatore
	for (i = 0; i < n_navi1; i++) {
		put_one (sea_HUMAN);
	}
	for (i = 0; i < n_navi2; i++) {
		put_two (sea_HUMAN);
	}
	for (i = 0; i < n_navi3; i++) {
		put_three (sea_HUMAN);
	}
	//Ciclo che regola il flusso del gioco (che termina quando uno dei partecipanti ha 0 navi)
	while ((sea_HUMAN->n_navi != 0) && (sea_CPU->n_navi != 0)) {
		//Stampa dei campi da gioco iniziali
		printf ("TURNO %d \n", N);
		putchar ('\n');
		print_area (sea_HUMAN);
		print_CPU (sea_CPU);
		//Mossa del giocatore
		printf ("Inserisci le coordinate della mossa (a b INVIO): \n");
		scanf ("%d %d", &a, &b);
		printf ("Colpo del giocatore in (%d, %d): ", a, b);
		stampa_esito (esegui_mossa (sea_CPU, a-1, b-1));
		//Mossa del computer (generazione casuale del colpo)
		sleep (1);
		a = rand()%sea_HUMAN->N;
		b = rand()%sea_HUMAN->M;
		printf ("Colpo del computer in (%d, %d): ", a+1, b+1);
		stampa_esito (esegui_mossa (sea_HUMAN, a, b));
		putchar ('\n');
		//Avanzamento del turno
		N++;
		}
	//Messaggio in caso di vittoria o di sconfitta
	if (sea_CPU->n_navi == 0) {
		printf ("CONGRATULAZIONI AMMIRAGLIO, ABBIAMO VINTO");
	}
	if (sea_HUMAN->n_navi == 0) {
		printf("BRUTTE NOTIZIE AMMIRAGLIO, SIAMO STATI SCONFITTI");
	}
	//Deallocamento delle matrici alla fine del gioco
	distruggi_area (sea_CPU);
	distruggi_area (sea_HUMAN);
}
bnavale.c
Visualizzazione di bnavale.c.