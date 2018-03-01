
#include <avr/io.h> 
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>

void initialisation ( void ) {


  DDRA = 0xff; // PORT A est en mode sortie
  DDRB = 0xff; // PORT B est en mode sortie
  DDRC = 0xff; // PORT C est en mode sortie
  DDRD = 0xff; // PORT D est en mode sortie

}


void ajustementPWM ( uint8_t nombre) {


// mise à un des sorties OC1A et OC1B sur comparaison

// réussie en mode PWM 8 bits, phase correcte

// et valeur de TOP fixe à 0xFF (mode #1 de la table 17-6

// page 177 de la description technique du ATmega324PA)

TCNT1 = 0 ;

OCR1A = nombre;

OCR1B = nombre;


// division d'horloge par 8 - implique une frequence de PWM fixe

TCCR1A |= (1 << COM1A1)| (1<< COM1B1); 
TCCR1A |= (1<< WGM10);
TCCR1B |= (1 << CS11);

TCCR1C = 0;

}


int main()
{
		initialisation();
		
		ajustementPWM (0);
		_delay_ms(2000);
		
		ajustementPWM (50);
		_delay_ms(2000);
		
		ajustementPWM (100);
		_delay_ms(2000);
		
		ajustementPWM (150);
		_delay_ms(2000);
		
		ajustementPWM (200);
		_delay_ms(2000);

}	
