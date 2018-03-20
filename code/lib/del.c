#include <avr/io.h>
#include <util/delay.h>
#include <del.h>

void del_init() {
	/* on initialise le PORTA en mode sortie */
	DDRA = 0xff;
}

void del_red(){
	/* On allume la DEL de la couleur rouge */
	PORTA = 0x02;
}

void del_green(){
	/* On allume la DEL de la couleur verte */
	PORTA = 0x01;
}

void del_amber(){
	/* On allume la DEL de la couleur ambrée */
	PORTA = 0x01;
	_delay_ms(1);
	PORTA = 0x02;
	_delay_ms(4);
}

void del_off(uint8_t numero){
	/* On éteint la DEL */
	PORTA &= ~numero;
}

void del_on(uint8_t numero) {
	/* On choisit le numéro de la PINA pour le mettre en mode sortie */
	PINA |= numero;

}