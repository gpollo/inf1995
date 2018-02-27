/*
 * Ce code prend en entré un boutton poussoir. 
 * Une fois pesée, il enclenche un compteur.
 * Ensuite, deux options sont posibles soient le 
 * bouton est relâché sinon le timer atteint une valeur 
 * de 120. Une fois que l'une de ces deux options est
 * atteinte la DEL clignote verte pendant 0.5 seconde.
 * Ensuite, il y a une attente de deux secondes. Suite 
 * à cette attente, la DEL clignote 2 fois par secondes
 * et cela pendant compteur/2. Finalement, la DEL passe 
 * au vert pendant 1 seconde et revient à son état initial.
*/
#define F_CPU 8000000UL
#define PRESCALER 1024

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DELAY_BOUNCE 30
#define DELAY_FLASH 100

#define LED_OFF   0x0
#define LED_RED   0x1
#define LED_GREEN 0x2

/** Les états possibles de ce programme. */
volatile enum states {
    INIT,
    WAIT_PRESSED,
    BLINK_GREEN,
    NOTHING,
    BLINK_RED,
    GREEN
} state = INIT;

/* Cette valeur est utilisée pour conserver une valeur du compteur. */
volatile uint16_t saved_counter;

/** Le compteur incrémenté dans le timer. */
volatile uint16_t counter = 0;

void control_led() {
    switch(state) {
        case INIT:
            counter = 0;
            PORTB = LED_OFF;

            /* pour changer d'état, il faut appuyer sur le bouton */
            if(!(PIND & 0x04)) {
                _delay_ms(DELAY_BOUNCE);
                state = WAIT_PRESSED;
            }   
            break; 

        case WAIT_PRESSED:
            /* pour changer d'état, il faut relâcher le bouton (ou timeout) */
            if(PIND & 0x04) {
                _delay_ms(DELAY_BOUNCE);
                state = BLINK_GREEN;
                saved_counter = counter;
            }
            break;

        case BLINK_GREEN:
            /* on utilise le compteur 100ms pour flasher pendant 500ms */
            if(counter >= saved_counter + 5) {
                state = NOTHING; 
                return;
            }

            _delay_ms(DELAY_FLASH);
            PORTB = LED_OFF;
            _delay_ms(DELAY_FLASH);
            PORTB = LED_GREEN;
            break;

        case NOTHING:
            PORTB = LED_OFF;
            saved_counter = counter/2;

            /* on ne fait qu'attendre 2 seconde avant de changer d'état */
            _delay_ms(2000);
            state = BLINK_RED;
            break;
        case BLINK_RED:
            PORTB = LED_OFF;
            _delay_ms(250);
            PORTB = LED_RED;
            _delay_ms(250);

            /* on utilise le compteur enregistré avant de changer d'état */
            saved_counter--;
            if(saved_counter == 0)
               state = GREEN;
            break;

        case GREEN:
            PORTB = LED_GREEN;

            /* on attend 1 seconde avant de retourner à l'initialisation */
            _delay_ms(1000);
            state = INIT;
            break;

        default:
            /* par défaut, on est à l'initialisation */
            state = INIT;
            break;
    }
}

/* interrupt Minuterie */
ISR(TIMER1_COMPA_vect) {
    cli();
    /* on change d'état seulement si le compteur est à 120 */
    if((counter++ == 120) && (state == WAIT_PRESSED)) {
        state = BLINK_GREEN;
        saved_counter = counter;
    }
    sei();
}

void init_timer() {
    TCNT1 = 0;

    /* environ 100ms pour 10Hz */
    OCR1A = 781;

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
    
    /* initialise l'interruption */
    cli();
    init_timer();
    sei();

    /* on boucle continuellement pour controller la LED */
    for(;;) control_led();


    return 0;
}
