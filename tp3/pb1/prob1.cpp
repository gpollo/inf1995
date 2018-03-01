/*Etat present| Entree D | Etat suivant | Sortie B |
 *==================================================
 * INIT       |    0     |     INIT     |    0     |
 * INIT       |    1     |     EA       |    0     |
 * EA         |    0     |     EA       |    0     |
 * EA         |    1     |     EB       |    0     |
 * EB         |    0     |     EB       |    0     |
 * EB         |    1     |     EC       |    0     |
 * EC         |    0     |     EC       |    0     |
 * EC         |    1     |     ED       |    0     |
 * ED         |    0     |     ED       |    0     |
 * ED         |    1     |     EE       |    1     |
 * EE         |    0     |     EE       |    0     |
 * EE         |    1     |     INIT     |    0     |
 *==================================================
 * INIT: Etat initial ou la DEL est eteinte au depart
 * EA: Etat ou le bouton a ete pesee la premiere fois
 * EB: Etat ou le bouton a ete pesee la deuxieme fois
 * EC: Etat ou le bouton a ete pesee la troisieme fois
 * ED: Etat ou le bouton a ete pesee la quatrieme fois
 * EE: Etat ou le bouton a ete pesee la cinquieme fois
 * */

#include <avr/io.h> 
#define F_CPU 8000000
#include <util/delay.h>

int main()
{
 
  DDRA = 0xff; // PORT A est en mode sortie
  DDRB = 0xff; // PORT B est en mode sortie
  DDRC = 0xff; // PORT C est en mode sortie
  DDRD = 0x00; // PORT D est en mode entree
  
uint8_t compteur = 0; 
PORTB = 0x00; // Le port B est initialement eteint

while(true){
if(PIND & 0x04) {
	_delay_ms (10);

		  switch (compteur) {
			  case 0:			  
			  case 1: 
			  case 2:
			  case 3: while (PIND & 0x04 ) { 
						PORTB = 0x00; 
						_delay_ms (1);
					}
					compteur++;
			  break;
			  case 4: while (PIND & 0x04 ) { // Donc, la DEL est rouge quand on pese 5 fois sur le bouton
						PORTB = 0x01; 
						_delay_ms (1);
					}
					compteur = 0;
			  break;
}
}
}

}
