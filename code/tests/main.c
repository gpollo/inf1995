#include <uart.h>
#include <memory.h>
#include <timer.h>
#include <adc.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <del.h>
#include <avr/io.h>

void init_interruption(void) {
	
	/* Il bloque toutes les interruptions */
    cli();
	
    /* on active l'interruption INT0 */
    EIMSK |= (1 << INT0);

    /* on configure sensiblité de l'interruption aux changements  */
    EICRA |= ISC00;
    EICRA |= ISC01;
    
	/* Il permet de recevoir à nouveau des interruptions*/
    sei();
}
volatile uint8_t compteur = 0;

volatile uint8_t boutton = 1;
ISR(INT0_vect) {;
    cli();
    boutton = (boutton == 0) ? 1 : 0;
    
    uart_putchar(0xf5);
    uart_putchar(boutton);
    _delay_ms(30);
    EIFR |= (1 << INTF0) ;
    sei();
}

int main(void) {
    uart_init();
	init_interruption();
    for(;;){
    }
    return 0;
}
