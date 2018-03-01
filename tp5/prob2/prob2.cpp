
#include <avr/io.h> 
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t minuterieExpiree;

volatile uint8_t boutonPoussoir;

void initialisation ( void ) {

cli ();

  DDRA = 0xff; // PORT A est en mode sortie
  DDRB = 0xff; // PORT B est en mode sortie
  DDRC = 0xff; // PORT C est en mode sortie
  DDRD = 0x00; // PORT D est en mode entree
  
  PORTB = 0x00; // Couleur rouge au depart
  //_delay_ms(10000);

EIMSK |= (1 << INT0) ;

EICRA |= _BV(ISC00);

//sei ();

}

ISR ( TIMER1_COMPA_vect ) {

minuterieExpiree = 1;
}


ISR ( INT0_vect ) {

boutonPoussoir = 1;

// anti-rebond
if (PIND & 0x04 ) {
		  _delay_ms (10);
		  
		  if (PIND & 0x04 ) {
		  boutonPoussoir = 1;
	  }
	  else{
	  boutonPoussoir = 0;
  }
}
}



void partirMinuterie ( uint16_t duree ) {

minuterieExpiree = 0;

// mode CTC du timer 1 avec horloge divisée par 1024

// interruption après la durée spécifiée

TCNT1 = 0 ;

OCR1A = duree;

TCCR1A |= _BV(COM1A0) ;

TCCR1B |= _BV(CS12)| _BV(CS10) ;

TCCR1C = 0;

TIMSK1 |= _BV(OCIE1A) ;

}


int main()
{
	initialisation();
	
_delay_ms(10000);
PORTB = 0x01;
_delay_ms(100);
PORTB = 0x00;
sei ();
partirMinuterie(8000000/1024);	

do {



// attendre qu'une des deux variables soit modifiée

// par une ou l'autre des interruptions.



}
 while ( minuterieExpiree == 0 && boutonPoussoir == 0 );

// Une interruption s'est produite. Arrêter toute

// forme d'interruption. Une seule réponse suffit.

cli ();

// Verifier la réponse
if(boutonPoussoir == 1){
PORTB = 0x02;
}
else if (boutonPoussoir == 0){
PORTB = 0x01;
}
}
