#include <interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <interrupt.h>
#include <util/delay.h>

void (*callback)(uint8_t, void*) = NULL;
void* data = NULL;

void interruption_init(void (*cb)(uint8_t, void*), void* cbdata) {
    /* on désactive les interruptions */
    cli();
	
    /* on active l'interruption INT0 */
    EIMSK |= _BV(INT0);

    /* on configure sensiblité de l'interruption aux changements */
    EICRA |= ISC00;
    EICRA |= ISC01;
    
    callback = cb;
    data = cbdata;
    
    /* on active les interruptions */
    sei();
}

/* par défaut, on assume que le bouton est relâché */
volatile uint8_t bouton = 0;
    
ISR(INT0_vect) {
    /* on désactive les interruptions */
    cli();
    
    /* on détermine l'orientation du 180 */
    if(callback != NULL) (*callback)((uint8_t) bouton, data);
    
    /* on applique un délais pour l'anti-rebond */
    _delay_ms(30);
    
    /* on réactive cet interruption */
    EIFR |= _BV(INTF0);
    
    /* on active les interruptions */
    sei();
}
