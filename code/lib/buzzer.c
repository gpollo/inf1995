#include <avr/io.h>
#include <util/delay.h>
#include <buzzer.h>

void buzzer_init(void) {
	/* on configure le compteur en CTC avec prescaler à 32*/
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(CS20) | _BV(CS21);
    
	/* faire partir le compteur à 0 */
	TCNT2 = 0;
	
	/* on active la pin du ground */
    SON_GROUND_DDR |= _BV(SON_GROUND_BIT);
    SON_GROUND_PORT &=~ _BV(SON_GROUND_BIT);

	/* on active la pin du son en sortie */
    SON_OUT_DDR |= _BV(SON_OUT_BIT);
}

void buzzer_jouer(void) {
   	
    /* on ajuste le PWM pour produire le son. OCR2A = 8 000 000 / (2 * 32 * 880) */
	OCR2A = 142; 
	
	/* toggle OC2A quand le TCNT2 atteint OCR2A */
	TCCR2A |= _BV(COM2A0);
}

void buzzer_arreter(void) {
    /* on désactive la pin de sortie du PWM */
	TCCR2A &= ~_BV(COM2A0);
}
