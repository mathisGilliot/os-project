#include <stdio.h>
#include <cpu.h>
#include <malloc.c.h>

#include "proc.h"
#include "time.h"

Processus *actif;
Processus *activablesTete;
Processus *activablesQueue;
Processus *endormisTete;
Processus *mourants;

static uint32_t npid = -1;

/* Fonctions d'accès à la structure des processus */

int32_t mon_pid(void){
    return actif->pid;
}

char* mon_nom(void){
    return actif->nom;
}

void dors(uint32_t nbr_secs){
    actif->reveil = nbr_secondes() + nbr_secs;
    ordonnance();
}

/* Ordonnancement */

void ordonnance(void){
    Processus *tmp = actif;
    Processus *prec = NULL;

    /* On libère les mourants */
    if (actif->etat != MOURANT){
	while(mourants != NULL){
	    if (prec != NULL){
		free(prec);
	    }
	    prec = mourants;
	    mourants = mourants->suiv;
	}
	if (prec != NULL){
	    free(prec);
	}
    }
	
    /* On réveille les dormeurs */
    if (endormisTete != NULL && endormisTete->reveil <= nbr_secondes()){
	Processus *d = extractionEndormi();
	while (d != NULL && d->reveil <= nbr_secondes()){
	    insertionActivable(d);
	    d = extractionEndormi();
	}
	if (d != NULL && d->reveil > nbr_secondes()){
	    insertionEndormi(d);
	}
    }
	
    /* L'ancien actif est ajouté à mourants, endormis ou activables */
    if (tmp->etat == MOURANT){
	tmp->suiv = mourants;
	mourants = tmp;
    }
    else if (tmp->reveil > nbr_secondes()){
	insertionEndormi(tmp);
    }
    else {
	insertionActivable(tmp);
    }

		
    /* Le nouvel actif */
    actif = extractionActivable();
    /* Échange des contextes si il y a un activable */
    if (actif != NULL){
	/* Contexte à Sauvegarder, Contexte à restaurer */
	ctx_sw(tmp->regs, actif->regs);
    }
    else {
	actif = tmp;
    }
}

/* Fonctions de gestion des listes */

void afficher(Processus *liste){
    Processus *cour = liste;
    while(cour != NULL){
	printf("%s->", cour->nom);
	cour = cour->suiv;
    }
    printf("NULL\n");
}

void insertionActivable(Processus *p){
    p->suiv = NULL;
    p->etat = ACTIVABLE;
    if(activablesTete == NULL){
	activablesTete = p;
    }
    else {
	activablesQueue->suiv = p;
    }
    activablesQueue = p;
}

Processus* extractionActivable(void){
    Processus *p = activablesTete;
    if (p == NULL){
	printf("erreur");
	return NULL;
    }
    activablesTete = activablesTete->suiv;
    p->etat = ELU;
    p->suiv = NULL;
    return p;
}

/* Attention : insertion avec condition */
void insertionEndormi(Processus *p){
    p->suiv = NULL;
    p->etat = ENDORMI;
    Processus *prec;
    Processus *cour = endormisTete;

    /* Si la liste est vide */
    if(endormisTete == NULL){
	endormisTete = p;
    }
    /* Sinon */
    else {
	/* Cas de l'insertion en tête */
	if (endormisTete->reveil > p->reveil){
	    p->suiv = endormisTete;
	    endormisTete = p;
	}
	else {
	    while(cour != NULL && cour->reveil <= p->reveil){
		prec = cour;
		cour = cour->suiv;
	    }
	    if (prec == NULL){
		printf ("Alerte");
	    }
	    else {
		prec->suiv = p;
	    }
	    p->suiv = cour;
	}
    }
}

Processus* extractionEndormi(){
    Processus *p = endormisTete;
    if (p == NULL){
	return NULL;
    }
    endormisTete = endormisTete->suiv;
    p->etat = ACTIVABLE;
    p->suiv = NULL;
    return p;
}

/* Fonctions relatives aux processus */

int32_t cree_processus(void (*code)(void), char *nom){
    npid++;
    Processus *p = malloc(sizeof(Processus));
    p->pid = npid;
    strcpy(p->nom, nom);
    if (npid == 0){
	p->etat = ELU;
	p->reveil = 0;
	actif = p;
    }
    else {
	p->etat = ACTIVABLE;
	p->regs[1] = (int)(&p->pile[TAILLE_PILE-2]);
	p->pile[TAILLE_PILE-2] = (int)(code);
	/* Pour gérer la terminaison */
	p->pile[TAILLE_PILE-1] = (int)(fin_processus);
	p->reveil = 0;
	insertionActivable(p);
    }
    table[npid] = p;
    return npid;
}

void fin_processus(void){
    actif->etat = MOURANT;
    table[actif->pid] = NULL;
    ordonnance();
}

void idle() {
    for (;;) {
	sti();
	hlt();
	cli();
    }
}

void proc1(void)
{
    for (int32_t i = 0; i < 2; i++) {
	printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
	       mon_nom(), mon_pid());
	dors(2);
    }  
}

void proc2(void)
{
    for (int32_t i = 0; i < 2; i++) {
	printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
	       mon_nom(), mon_pid());
	dors(3);
    }
    cree_processus(proc3, "proc3");
}

void proc3(void)
{
    printf("%s", table[3]->nom);
    for (;;) {
	printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
	       mon_nom(), mon_pid());
	dors(5);
    }
}
