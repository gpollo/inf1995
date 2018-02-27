/*
 * +---------+----------+---------+--------+
 * |  État   |  Bouton  |  État   | Sortie |
 * | Présent | Poussoir | Suivant |  LED   |
 * +---------+----------+---------+--------+
 * | INIT    | relaché  |   INIT  | eteint |
 * | INIT    | appuyer  |  CLICK1 | eteint |
 * | CLICK1  | relaché  |  CLICK1 | eteint |
 * | CLICK1  | appuyer  |  CLICK2 | eteint |
 * | CLICK2  | relaché  |  CLICK2 | eteint |
 * | CLICK2  | appuyer  |  CLICK3 | eteint |
 * | CLICK3  | relaché  |  CLICK3 | eteint |
 * | CLICK3  | appuyer  |  CLICK4 | eteint |
 * | CLICK4  | relaché  |  CLICK4 | eteint |
 * | CLICK4  | appuyer  |  CLICK5 | eteint |
 * | CLICK5  | relaché  |  CLICK5 | eteint |
 * | CLICK5  | appuyer  |  ALLUME | allume |
 * | ALLUME  | relaché  |  ALLUME | ALLUME |
 * | ALLUME  | appuyer  |  INIT   | eteint |
 * +---------+----------+---------+--------+
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

#define DELAY_LIGHT 1000
#define DELAY_BOUNCE 10
#define BUTTON_PRESSED 0x04
#define BUTTON_RELEASED 0x00
#define LED_RED 0x1
#define LED_OFF 0x0

enum states : uint8_t {
    eteint,
    allume
} state = eteint;

void state_machine(void) {
    uint8_t compteur;
    uint8_t button_status = BUTTON_RELEASED;

    switch (state) {
        case eteint:
            compteur = 0;
            while(compteur < 5){ // Sort quand atteint valeur allume
                uint8_t value = PIND & 0x04; // mask pour boutton poussoir

                if(value == button_status) // Tant que pas presser boucler
                    continue;

                button_status = value; // State HIGH
                _delay_ms(DELAY_BOUNCE);

                if(button_status == BUTTON_PRESSED) // Check if stayed high
                    compteur++;
            }

            state = allume;
            break;
        case allume:
            PORTB = LED_RED;
            _delay_ms(DELAY_LIGHT);
            PORTB = LED_OFF;
        default:
            state = eteint; 
            break;
        }
}

int main() {
    DDRB = 0xff;
    DDRD = 0x00;
    PORTB = LED_OFF;

    for(;;) state_machine();

    return 0;
}
