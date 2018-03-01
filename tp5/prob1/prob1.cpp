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
#include <avr/interrupt.h>

volatile uint8_t etat = 0; 
volatile uint8_t minuterieExpiree;
//volatile uint8_t compteur = 0;

/*bool BoutonPoussoir()
{
	if (PIND & 0x04){
		_delay_ms(30);

		if (PIND & 0x04){
			return true;
		}
	
	//while (PIND & 0x00){
	//	_delay_ms(30);
		else {
			return false;
		}
}

}

*/

// placer le bon type de signal d'interruption

// à prendre en charge en argument
void initialisation ( void ) {

// cli est une routine qui bloque toutes les interruptions.

// Il serait bien mauvais d'être interrompu alors que

// le microcontroleur n'est pas prêt...

cli ();


// configurer et choisir les ports pour les entrées

// et les sorties. DDRx... Initialisez bien vos variables

  DDRA = 0xff; // PORT A est en mode sortie
  DDRB = 0xff; // PORT B est en mode sortie
  DDRC = 0xff; // PORT C est en mode sortie
  DDRD = 0x00; // PORT D est en mode entree
  
  PORTB = 0x01; // Couleur rouge au depart

// cette procédure ajuste le registre EIMSK de l’ATmega324PA pour permettre les interruptions externes

EIMSK |= (1 << INT0) ;

// il faut sensibiliser les interruptions externes aux
// changements de niveau du bouton-poussoir
// en ajustant le registre EICRA

EICRA |= _BV(ISC00);


// sei permet de recevoir à nouveau des interruptions.

sei ();

}
volatile uint8_t compteur = 0;

ISR(INT0_vect) {
	//initialisation();
	//bool bouton = BoutonPoussoir();
// laisser un delai avant de confirmer la réponse du bouton-poussoir: environ 30 ms (anti-rebond)
//_delay_loop_ms ( 30 );  
// se souvenir ici si le bouton est pressé ou relâché

// changements d'états tels que ceux de la semaine précédent
	//if (bouton == true){
	
	  if (PIND & 0x04 ) {
		  _delay_ms (30);
		  
	  switch (compteur) {
			  case 0: 
					while(PIND & 0x04) {
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
						while (PIND & 0x04) {
						PORTB = 0x02; // couleur verte
						_delay_ms (1);
					}
				
						PORTB= 0x01; // couleur rouge
					compteur = 0;
				
			  break;
}

EIFR |= (1 << INTF0) ;
}
}

		 

// Voir la note plus bas pour comprendre cette instruction et son rôle



 int main()
{
	initialisation();
	for(;;){
	}
	
}


