/*
 * +---------+----------+---------+--------+
 * |  État   |  Bouton  |  État   | Sortie |
 * | Présent | Poussoir | Suivant |  LED   |
 * +---------+----------+---------+--------+
 * | INIT    | relaché  |  INIT   | rouge  |
 * | INIT    | appuyer  |  E1     | ambre  |
 * | E1      | appuyer  |  E1     | ambre  |
 * | E1      | relaché  |  E2     | vert   |
 * | E2      | relaché  |  E2     | vert   |
 * | E2      | appuyer  |  E3     | rouge  |
 * | E3      | appuyer  |  E3     | rouge  |
 * | E3      | relaché  |  E4     | eteint |
 * | E4      | relaché  |  E4     | eteint |
 * | E4      | appuyer  |  E5     | vert   |
 * | E5      | appuyer  |  E5     | vert   |
 * | E5      | relaché  |  INIT   | rouge  |
 * +---------+----------+---------+--------+
 */
#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DELAY_BOUNCE 30
#define DELAY_AMBRE  5

#define LED_OFF   0x0
#define LED_RED   0x1
#define LED_GREEN 0x2

#define BUTTON_RELEASED 0
#define BUTTON_PRESSED  1

volatile enum states {
    INIT,
    E1,
    E2,
    E3,
    E4,
    E5
} state = INIT;

void state_machine(uint8_t button) {
	switch (state) {
        case INIT:
            if(button == BUTTON_PRESSED) state = E1;
            break;
        case E1:
            if(button == BUTTON_RELEASED) state = E2;
            break;
        case E2:
            if(button == BUTTON_PRESSED) state = E3;
            break;
        case E3:
            if(button == BUTTON_RELEASED) state = E4;
            break;
        case E4:
            if(button == BUTTON_PRESSED) state = E5;
            break;
        case E5:
            if(button == BUTTON_RELEASED) state = INIT;
            break;
        default:
            state = INIT;
            break;
	}
}

void control_led() {
    switch(state) {
        case INIT:
            PORTB = LED_RED;
            break; 
        case E1:
            PORTB = LED_RED;
            _delay_ms(DELAY_AMBRE);
            PORTB = LED_GREEN;
            _delay_ms(DELAY_AMBRE); 
            break;
        case E2:
            PORTB = LED_GREEN;
            break;
        case E3:
            PORTB = LED_RED;
            break;
        case E4:
            PORTB = LED_OFF;
            break;
        case E5:
            PORTB = LED_GREEN;
            break;
        default:
            state = INIT;
            break;
    }

}

/* au début, on assume que le bouton n'est pas appuyé */
volatile uint8_t button = BUTTON_RELEASED;

ISR(INT0_vect) {
    cli();

    /* délais pour empêcher le bouncing */
    _delay_ms(DELAY_BOUNCE);

    /* on update l'état du bouton */
    button = (button == BUTTON_RELEASED) ? BUTTON_PRESSED : BUTTON_RELEASED;
    
    /* on update la machine à état */
    state_machine(button);

    /* activate the next interrupt */
    EIFR |= (1 << INTF0);

    sei();
}

void init_interruption(void) {
    cli();

    /* on active l'interruption INT0 */
    EIMSK |= (1 << INT0);

    /* on configure sensiblité de l'interruption aux changements  */
    EICRA |= ISC00;
    EICRA |= ISC01;

    sei();
}

int main() {
    /* sortie de la LED */
    DDRB = 0xff;

    /* initialise l'interruption */
    init_interruption();

    /* on boucle continuellement pour controller la LED */
    for(;;) control_led();

    return 0;
}
