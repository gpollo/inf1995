/*
 * Ecole Polytechnique de Montreal
 * Departement de genie informatique
 * Cours inf1995
 *
 * Dalyna Pak et William Balea
 * Équipe 62
 * 
 * TP7 : Utilisation du bouton-poussoir
 * Date: 27 février 2018
 */
#include <avr/io.h> 
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t boutonPoussoir;
volatile int compteur;

void initialisation ( void ) {

cli (); //Empêcher les interruptions

  DDRA = 0xff; // PORT A est en mode sortie
  DDRB = 0xff; // PORT B est en mode sortie
  DDRC = 0xff; // PORT C est en mode sortie
  DDRD = 0x00; // PORT D est en mode entree
  
EIMSK |= (1 << INT0); // Permettre les interruptions externes après que le bouton est appuyé

EICRA |= _BV(ISC00); // interruption externe par le bouton

sei();//Accepter les interruptions

}

ISR ( TIMER1_COMPA_vect ) {

compteur++; //Le compteur incrémente lorsque le bouton poussoir
}

ISR ( INT0_vect ) {
	
	_delay_ms(30); //anti-rebond lorsque le bouton est appuyé
	boutonPoussoir = 1;
		  
	 if (PIND & 0x04 ) { //Si le bouton est relâché
		boutonPoussoir = 0;
  }
}

void partirMinuterie ( uint16_t duree ) {

TCNT1 = 0 ; // Partir le compteur à 0

OCR1A = duree; // Permet de compter à la valeur de duree

TCCR1A |= _BV(COM1A0) ; // mode CTC du timer 1 avec horloge divisée par 1024

TCCR1B |= _BV(CS12)| _BV(CS10) | _BV(WGM12) ; // mode CTC du timer 1 avec horloge divisée par 1024

TCCR1C = 0; // mode CTC du timer 1 avec horloge divisée par 1024

TIMSK1 |= _BV(OCIE1A) ; // Timer/Counter1: interruption après que la duree est atteinte par le compteur


}

int main()
{
	initialisation();
	for(;;){
	compteur = 0;
	if(boutonPoussoir==1){
	partirMinuterie(781);	// (8000000/1024/10) : compteur (prescaler de 1024) qui sera incrémenté 10 fois / sec
do {

}
 while ( compteur<120 && boutonPoussoir==1); //Tant que le compteur n'a pas atteint 120 ni qu'il relâche le bouton

	TIMSK1 = 0; //Le TIMSK1 arrête de compter
	PORTB = 0x02; //La DEL devient vert pour 1/2 secondes
	_delay_ms(500);
	PORTB = 0x00;   // La DEL est éteinte pendant 2 secondes
	_delay_ms(2000);
	
	for (uint8_t i = 0; i < compteur/2; i++) // La DEL devient ambrée en fonction du compteur/2
	{
		PORTB = 0x01;
		_delay_ms(250);
		PORTB = 0x00;
		_delay_ms(250);
	}
	PORTB = 0x02; //La DEL devient ainsi verte pendant 1 seconde
	_delay_ms(1000);
	
	PORTB = 0x00; //La DEl est éteinte et on peut recommencer la procédure
}
}
}


