#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>

#include "start.h"
#include "time.h"
#include "proc.h"


uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1) {
        res = 1;
    } else {
        res = fact(n - 1) * n;
    }
    return res;
}

void kernel_start(void)
{
  /* On nettoie l'écran */
  printf("\f");

  uint32_t x = fact(5);
  /* printf("%d",x); */
  /* Pour utiliser x (werror sinon) */
  (void)x;
 
  /* Initialisation */
  reglageHorloge();
  init_traitant_IT(32, &traitant_IT_32);
  masque_IRQ(0, false);

  cree_processus(idle, "idle");
  cree_processus(proc1, "proc1");
  cree_processus(proc2, "proc2");
  //cree_processus(proc3, "proc3");

    
  /* Démasquage des interruptions externes */
  //sti();

  /* Lancement processus par défaut */
  idle();
  
  while (1) {
    /* Cette fonction arrete le processeur */
    hlt();
  }
}

