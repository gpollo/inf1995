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

#define DELAY_BOUNCE 10
#define DELAY_AMBRE  5

#define LED_OFF   0x0
#define LED_RED   0x1
#define LED_GREEN 0x2

#define button_pressed (PIND & 0x04)

enum states {
    INIT,
    E1,
    E2,
    E3,
    E4,
    E5
} state = INIT;

void state_machine(void) {
	switch (state){
        case INIT:
            PORTB = LED_RED;
            if(button_pressed) state = E1;
            break;
        case E1:
            while(button_pressed) {
                PORTB = LED_RED;
                _delay_ms(DELAY_AMBRE);
                PORTB = LED_GREEN;
                _delay_ms(DELAY_AMBRE);
            }

            _delay_ms(DELAY_BOUNCE);

            if(!button_pressed) state = E2;
            break;
        case E2:
            while(!button_pressed) PORTB = LED_GREEN;

            _delay_ms(DELAY_BOUNCE);

            if(button_pressed) state = E3;
            break;
        case E3:
            while(button_pressed) PORTB = LED_RED;

            _delay_ms(DELAY_BOUNCE);

            if(!button_pressed) state = E4;
            break;
        case E4:
            while(!button_pressed) PORTB = LED_OFF;

            _delay_ms(DELAY_BOUNCE);

            if(button_pressed) state = E5;
            break;
        case E5:
            while(button_pressed) PORTB = LED_GREEN;

            _delay_ms(DELAY_BOUNCE);

            if(!button_pressed) state = INIT;
            break;
        default:
            state = INIT;
            break;
	    }
}

int main() {
    DDRB = 0xff;
    DDRD = 0x00;

    for(;;) state_machine();

    return 0;
}
