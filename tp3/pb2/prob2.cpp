/*Etat present| Entree D | Etat suivant | Sortie B |
 *==================================================
 * INIT       |    0     |     INIT     |    01    |
 * INIT       |    1     |     EA       |    11    |
 * EA         |    0     |     EA       |    10    |
 * EA         |    1     |     EB       |    01    |
 * EB         |    0     |     EB       |    00    |
 * EB         |    1     |     EC       |    10    |
 * EC         |    0     |     INIT     |    01    |
 *==================================================
 * Precision:
 * 01 : rouge
 * 10 : vert
 * 11 : ambre
 * 00 : eteint
 * INIT: Etat initial ou la DEL est eteinte au depart
 * EA: Etat ou le bouton a ete pesee la premiere fois
 * EB: Etat ou le bouton a ete pesee la deuxieme fois
 * EC: Etat ou le bouton a ete pesee la troisieme fois
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
  
  PORTB = 0x01; // Couleur rouge au depart
  uint8_t compteur = 0;
  
  while (true) {
	  if (PIND & 0x04 ) {
		  _delay_ms (10);
		  
		  switch (compteur) {
			  case 0: 
					while (PIND & 0x04 ) {
						PORTB = 0x01; // couleur ambree
						_delay_ms (1);
						PORTB = 0x02; 
						_delay_ms (1);
					}
						PORTB = 0x02; // couleur verte	
			compteur++;	
			
			break;  
			  case 1:
						while (PIND & 0x04 ) {
						PORTB = 0x01; // couleur rouge
						_delay_ms (1);
					}
						PORTB = 0x00; // couleur incolore
			compteur++;	
			 
			 break; 
			 
			  case 2:
						while (PIND & 0x04 ) {
						PORTB = 0x02; // couleur verte
						_delay_ms (1);
					}
				
						PORTB= 0x01; // couleur rouge
					compteur = 0;
				
			  break;
}
}

}
}


