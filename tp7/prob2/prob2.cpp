/*
 * Ecole Polytechnique de Montreal
 * Departement de genie informatique
 * Cours inf1995
 *
 * Dalyna Pak et William Balea
 * Équipe 62
 * 
 * TP7 : Utilisation de la photorésistance
 * Date: 27 février 2018
 */
 
#include <avr/io.h> 
#define F_CPU 8000000
#include <util/delay.h>
#include "can.h"

void initialisation ( void ) {

  DDRA = 0x00; // PORT A est en mode entree
  DDRB = 0xff; // PORT B est en mode sortie
  DDRC = 0xff; // PORT C est en mode sortie
  DDRD = 0x00; // PORT D est en mode entree
  
}


int main()
{
	initialisation();
	can capteur; //Création d'un objet
	
	for(;;){
	uint8_t i =(capteur.lecture(PINA) >> 2); //Décalage de 2 bits pour obtenir 8 bits les plus significatifs
	
	if(i < 130){ // La DEL devient verte lorsque la lumière est basse
	PORTB = 0x02; 
}
	if ( i>=130 && i <235){ // La DEL devient ambrée lorsque la lumière à un bon niveau
	PORTB = 0x01;
	_delay_ms(1);
	PORTB = 0x02;
	_delay_ms(1);
}
	if ( i >= 235){ // La DEL devient rouge lorsque la lumière est trop forte
	PORTB = 0x01;
}
}

}

