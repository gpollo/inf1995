#include <avr/io.h>
#include <util/delay.h>
#include <del.h>

void del_init(void ) {
	/* on initialise le PORTA en mode sortie */
	DDRA = 0xff;
}

void del_red(void) {
	/* on allume la DEL de la couleur rouge */
	PORTA = 0x02;
}

void del_green(void) {
	/* on allume la DEL de la couleur verte */
	PORTA = 0x01;
}

void del_amber(void) {
	/* on allume la DEL de la couleur ambrée */
	PORTA = 0x01;
	_delay_ms(1);
	PORTA = 0x02;
	_delay_ms(4);
}

void del_off(uint8_t numero) {
	/* on éteint la DEL */
	PORTA &= ~numero;
}

void del_on(uint8_t numero) {
	/* on choisit le numéro de la PINA pour le mettre en mode sortie */
	PORTA |= numero;
}

void del_depart() {
    /* droite à gauche */
    for(uint8_t j = 0; j < 10; j++) {
        for(uint8_t i = 0; i < 7; i++) {
            PORTA = _BV(0) << i;
            _delay_ms(20);
        }

        for(uint8_t i = 0; i < 7; i++) {
            PORTA = _BV(7) >> i;
            _delay_ms(20);
        }
    }

    /* compteur */
    for(uint16_t i = 0; i < 256; i++) {
        PORTA = i;
        _delay_ms(15);
    }

    /* droite à gauche et gauche à droite */
    for(uint8_t j = 0; j < 20; j++) {
         for(uint8_t i = 0; i < 3; i++) {
            PORTA = (_BV(0) << i) | (_BV(7) >> i);
        _delay_ms(40);
         }

         for(uint8_t i = 0; i < 3; i++) {
            PORTA = (_BV(3) >> i) | (_BV(4) << i);
            _delay_ms(40);
         }
    }
}
