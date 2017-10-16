#ifndef __TIME_H__
#define __TIME_H__

#include <stdio.h>
#include <stdbool.h>
#include <segment.h>
#include "console.h"

#define QUARTZ 0x1234DD
#define CLOCKFREQ 50

uint32_t nbr_secondes(void);

void printHeure(const char *s);

void tic_PIT(void);

void init_traitant_IT(int32_t num_IT, void (*traitant)(void));

void masque_IRQ(uint32_t num_IRQ, bool masque);

void reglageHorloge();

#endif
