#include "time.h"
#include "proc.h"

static int clock = 0;
static int heures = 0;
static int minutes = 0;
static int secondes = 0;
static uint32_t sec_total = 0;

uint32_t nbr_secondes(){
	return sec_total;
}

void printHeure(const char* s){
	for (uint32_t i = 0; i < 8; i++){
		ecrit_car(0, 72+i, s[i]);
	}
}

void tic_PIT(void){
	outb(0x20, 0x20);
	clock ++;
	if (clock % CLOCKFREQ == 0){
		sec_total ++;
		secondes ++;
		if (secondes == 60){
			minutes ++;
			secondes = 0;
			if (minutes == 60){
				heures ++;
				minutes = 0;
			}
		}
	}
	char* chaine = "00:00:00";
	sprintf(chaine, "%02i:%02i:%02i", heures, minutes, secondes);
	printHeure(chaine);
	ordonnance();
}

/* Pour initialiser la table des vect d'interruptions IDT */
void init_traitant_IT(int32_t num_IT, void (*traitant)(void)){
  /* Adresse du vecteur d'interruption 32 */
  /*  Comptage en octets (8 * num_IT octets après) */
  uint32_t *adresseI32 = (uint32_t*)(0x1000 + 8 * num_IT);
  *adresseI32 = (KERNEL_CS << 16) | ((uint32_t)(traitant) & 0xFFFF);
  /* Pour écrire le mot suivant 32 bits plus loin (+ 1 pointé) */
  *(adresseI32 + 1) = ((uint32_t)(traitant) & 0xFFFF0000) | 0x8E00;
}

void masque_IRQ(uint32_t num_IRQ, bool masque){
  uint8_t t_masque = inb(0x21);
  if( (t_masque >> num_IRQ)%2 == 1){
    if(!masque){
      t_masque -= 1 << num_IRQ;
    }
  }
  else {
    if(masque){
      t_masque += 1 << num_IRQ;
    }
  }
  outb(t_masque, 0x21);
}

void reglageHorloge(){
  outb(0x34, 0x43);
  outb((QUARTZ / CLOCKFREQ) % 256, 0x40);
  outb((QUARTZ / CLOCKFREQ) >> 8, 0x40);
}
