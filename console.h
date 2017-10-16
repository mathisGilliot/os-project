#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <inttypes.h>
#include <cpu.h>
#include <string.h>

/*
 * This is the function called by printf to send its output to the screen. You
 * have to implement it in the kernel and in the user program.
 */

extern void console_putbytes(const char *s, int len);
uint16_t *ptr_mem(uint32_t lig, uint32_t col);
void ecrit_car(uint32_t lig, uint32_t col, char c);
void efface_ecran(void);
void place_curseur(uint32_t lig, uint32_t col);
void traite_car(char c);
void defilement(void);

#endif
