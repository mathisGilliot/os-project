#ifndef __PROC_H__
#define __PROC_H__

#include <inttypes.h>

#define NB_PROCESSUS 10
#define TAILLE_NOM 15
#define TAILLE_PILE 512

typedef enum {ELU, ACTIVABLE, ENDORMI, MOURANT} Etat;

typedef struct processus{
	int32_t pid;
	char nom[TAILLE_NOM];
	Etat etat;
	int32_t regs[5];
	int32_t pile[TAILLE_PILE];
	uint32_t reveil;
	struct processus *suiv;
} Processus;

Processus* table[NB_PROCESSUS];

void afficher(Processus *liste);

void ctx_sw(int* regsAncien, int* regsNouveau);

int32_t mon_pid(void);

char* mon_nom(void);

void ordonnance(void);

void insertionActivable(Processus *p);

Processus* extractionActivable(void);

void insertionEndormi(Processus *p);

Processus* extractionEndormi();

int32_t cree_processus(void (*code)(void), char *nom);

void fin_processus(void);

void idle(void);

void proc1(void);

void proc2(void);

void proc3(void);

#endif
