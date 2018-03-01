#include <avr/io.h> 
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include "memoire_24.h"


int main()
{
  DDRA = 0xff; // PORT A est en mode sortie
  DDRB = 0xff; // PORT B est en mode sortie
  DDRC = 0xff; // PORT C est en mode sortie
  DDRD = 0xff; // PORT D est en mode entree
  
  PORTB = 0x00; 
  
Memoire24CXXX memoire;

uint8_t tabEcriture[] = "*E*C*O*L*E* *P*O*L*Y*T*E*C*H*N*I*Q*U*E*";

memoire.ecriture(0x0000, tabEcriture, 40*sizeof(uint8_t));
_delay_ms(5);

uint8_t tabLecture[] = {0};
memoire.lecture(0x0000,tabLecture, 40*sizeof(uint8_t));

bool estPareil = true;

for (int i = 0; i< 40; i++)
{
if(tabEcriture[i] != tabLecture[i])
estPareil = false;
}
if (estPareil){
	for(;;)
	PORTB =  0x02;
}
else {
for(;;)
PORTB = 0x01;
}
}
