#include <avr/io.h>
#include <util/delay.h>
#include <son.h>
#include <son_data.h>

/** La table des sons. */
uint16_t son_table[] = SON_DATA;

void son_init(void) {
	/* on configure le compteur en CTC avec prescaler à 1*/
    TCCR1A = 0;
    TCCR1B = _BV(CS00) | _BV(WGM12);
    
	/* faire partir le compteur à 0 */
	TCNT1 = 0;
	
	/* on active la pin du ground */
    SON_GROUND_DDR |= _BV(SON_GROUND_BIT);
    SON_GROUND_PORT &=~ _BV(SON_GROUND_BIT);

	/* on active la pin du son en sortie */
    SON_OUT_DDR |= _BV(SON_OUT_BIT);
}

void son_jouer(uint8_t numero) {
    /* on s'assure que la valeur demandée est dans la table */
	if(numero < SON_MIN || numero > SON_MAX) return;
	
    /* on ajuste le PWM pour produire le son */
	OCR1A = son_table[numero-SON_MIN];
	
	/* toggle OC1A quand le TCNT1 atteint OCR1A */
	TCCR1A |= _BV(COM1A0);
}

void son_arreter(void) {
    /* on désactive la pin de sortie du PWM */
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
