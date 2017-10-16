#include "console.h"

uint8_t cligno = 0b0;
uint8_t cfond = 0b000;
uint8_t ctexte = 0b1111;

uint8_t ligne = 0;
uint8_t colonne = 0;

uint16_t *ptr_mem(uint32_t lig, uint32_t col){
  if ((lig >= 0) & (lig <= 24) & (col >= 0) & (col <= 79)){
    return (uint16_t*)(0xB8000 + 2 * (80 * lig + col));
  }
  else
    return NULL;
}

void ecrit_car(uint32_t lig, uint32_t col, char c){
	uint16_t *adresse = ptr_mem(lig, col);
	uint8_t style = (cligno << 7) + (cfond << 4) + ctexte;
	*adresse = (uint16_t)(style << 8) + (uint8_t)c;
}

void efface_ecran(void){
  // Traitement particulier pour la 1ere ligne du à l'heure
  for(uint16_t j = 0; j < 72; j++){
      ecrit_car(0, j, ' ');
    }
  // Pour le reste
  for(uint16_t i = 1; i < 25; i++){
    for(uint16_t j = 0; j < 80; j++){
      ecrit_car(i, j, ' ');
    }
  }
}

void place_curseur(uint32_t lig, uint32_t col){
	uint16_t pos = 80 * lig + col;
	outb(0x0F, 0x3D4);
	outb((uint8_t)pos, 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t)(pos >> 8), 0x3D5);
}

void traite_car(char c){
  if (c < 127){
    switch(c){
    case 8:
      if (colonne > 0){
	colonne --;				
      }
      break;
    case 9 :
      if (colonne < 72){
	colonne += 8;
      }
      break;
    case 10:
	ligne ++;
	colonne = 0;
      break;
    case 12:
      efface_ecran();
      ligne = 0;
      colonne = 0;
      break;
    case 13:
      colonne = 0;
      break;
      
    default:
      if (c > 31){
	ecrit_car(ligne, colonne, c);
	colonne ++;
	
      }
    }
  }
  if (colonne == 80){
    ligne ++;
    colonne = 0;
  }
  if (ligne == 25){
    defilement();
    ligne = 24;
  }
  // Pour l'affichage de l'heure
  if ((ligne == 0) & (colonne == 72)){
    ligne = 1;
    colonne = 0;
  }
  place_curseur(ligne, colonne);
}

void defilement(void){
  // Traitement spécial 1ere ligne
  for (uint16_t j = 0; j < 72; j ++){
    memmove(ptr_mem(0, j), (void*)ptr_mem(1, j), 2);
  }
  // On copie une ligne au dessus de l'autre
  for (int16_t i = 2; i < 25; i ++){
    memmove((void*)ptr_mem(i-1, 0), (void*)ptr_mem(i, 0), 2*80);
  }
  // Puis on efface la dernière
  for (uint16_t j = 0; j < 80; j++){
    ecrit_car(24, j, ' ');
  }
}

void console_putbytes(const char *s, int len){
	for (uint32_t i = 0; i < len; i++){
		traite_car(s[i]);
	}
}



