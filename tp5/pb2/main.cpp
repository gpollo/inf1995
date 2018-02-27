#define F_CPU 8000000UL
#define PRESCALER 1024

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DELAY_BOUNCE 30
#define DELAY_INIT 10000
#define DELAY_FLASH 100

#define LED_OFF   0x0
#define LED_RED   0x1
#define LED_GREEN 0x2

#define BUTTON_RELEASED 0
#define BUTTON_PRESSED  1

volatile enum states {
    WAITING,
    SUCCESS,
    FAIL
} state = WAITING;

void control_led() {
    switch(state) {
        case WAITING:
            PORTB = LED_OFF;
            break; 
        case SUCCESS:
            PORTB = LED_GREEN;
            break;
        case FAIL:
            PORTB = LED_RED;
            break;
        default:
            break;
    }

}

/* interrupt Button */
ISR(INT0_vect) {
    cli();

    /* délais pour empêcher le bouncing */
    _delay_ms(DELAY_BOUNCE);

    /* on update l'état */
    if(state == WAITING) state = SUCCESS;

    sei();
}

/* interrupt Minuterie */
ISR(TIMER1_COMPA_vect){
    cli();

    /* on update l'état */
    if(state == WAITING) state = FAIL;

    sei();
}

void init_interruption(void) {
    /* on active l'interruption INT0 */
    EIMSK |= (1 << INT0);

    /* on configure sensiblité de l'interruption aux changements  */
    EICRA |= ISC00;
    EICRA |= ISC01;
}

void init_timer(uint16_t duree){
    TCNT1 = 0;

    /* on calcul le temps d'attente */
    OCR1A = (duree*F_CPU)/PRESCALER;

    /* comportement normal */
    TCCR1A = 0;
    
    /* chaque 1024 front d'horloge va incrementer */
    TCCR1B = (1 << CS12) | (1 << CS10) | (1 << WGM12);
    TCCR1C = 0;
    TIMSK1 = (1 << OCIE1A);
}

int main() {
    /* sortie de la LED */
    DDRB = 0xff;
    
    _delay_ms(DELAY_INIT);
    PORTB = LED_RED;
    _delay_ms(DELAY_FLASH);
    PORTB = LED_OFF;
    
    /* initialise l'interruption */
    cli();
    init_interruption();
    init_timer(1);
    sei();

    /* on boucle continuellement pour controller la LED */
    for(;;) control_led();


    return 0;
}
