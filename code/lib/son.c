#include <avr/io.h>
#include <util/delay.h>
#include <son.h>

void son_init(void) {
	/* on configure le compteur en CTC avec prescaler à 1*/
    TCCR1A = 0;
    TCCR1B = _BV(CS00) | _BV(WGM12);
    
	/* faire partir le compteur à 0 */
	TCNT1 = 0;
	
	/* on active la pin 4 comme ground */
	DDRD |= _BV(4);
	PORTD &= ~_BV(4);

	/* on active la pin 5 en sortie */
    OC1A_DDR |= _BV(OC1A_BIT);
}

uint16_t son_table[37] = {	
	36362, /* 45 */
	34321, /* 46 */
	32395, /* 47 */
	30577, /* 48 */
	28860, /* 49 */
	27241, /* 50 */
	25711, /* 51 */
	24268, /* 52 */
	22906, /* 53 */
	21620, /* 54 */
	20407, /* 55 */
	19261, /* 56 */
	18180, /* 57 */
	17160, /* 58 */
	16197, /* 59 */
	15288, /* 60 */
	14429, /* 61 */
	13619, /* 62 */
	12855, /* 63 */
	12133, /* 64 */
	11452, /* 65 */
	10809, /* 66 */
	10203, /* 67 */
	 9630, /* 68 */
	 9089, /* 69 */
	 8579, /* 70 */
	 8098, /* 71 */
	 7643, /* 72 */
	 7214, /* 73 */
	 6809, /* 74 */
	 6427, /* 75 */
	 6066, /* 76 */
	 5725, /* 77 */
	 5404, /* 78 */
	 5101, /* 79 */
	 4814, /* 80 */
	 4544, /* 81 */
};

void son_jouer(uint8_t numero) {
	if(numero < 45 || numero > 81) return;
	
	numero -= 45;
	OCR1A = son_table[numero];
	
	/* toggle OC1A quand le TCNT1 atteint OCR1A */
	TCCR1A |= _BV(COM1A0);
}

void son_arreter(void) {
	TCCR1A &= ~_BV(COM1A0);
}

void son_depart(void) {
	son_jouer(70);
	_delay_ms(400);
	son_arreter();
	_delay_ms(80);
	
	son_jouer(65);
	_delay_ms(600);
	son_arreter();
	_delay_ms(120);
	
	son_jouer(70);
	_delay_ms(250);
	son_arreter();
	_delay_ms(50);
	
	son_jouer(70); 
	_delay_ms(130);
	son_jouer(72); 
	_delay_ms(130);
	son_jouer(74);
	_delay_ms(130);
	son_jouer(75);
	_delay_ms(130);
	son_jouer(77);
	_delay_ms(500);
	son_arreter();
}
