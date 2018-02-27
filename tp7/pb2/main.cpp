/* 
 * Ce code change la couleur en sortie de la DEL,
 * selon le niveau d'exposition a la lumiere détecté par
 * la photorésistance.
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include "can.h"

/*
 * Note : Notre robot atteint seulement 4.4 volts en tournant la
 *        la vis du potentiomètre. Voici les valeurs correspontes 
 *        pour cette référence. 
 */
#define BASSE    700
#define AMBIANTE 950

#define LED_GREEN 0x01
#define LED_RED   0x02

#define DELAY_AMBRE 10

int main() {
    /* on active le Port B en sortie */
    DDRB = 0xff;

    /* on crée un objet de type can */
    can can;
    
    while(1) {
        /* on initialise la lecture a PA0 */
        uint16_t value = can.lecture(0);

        /* valeur de la DEL selon l'exposition de luminosité */
        if(value < BASSE) {
            PORTB = LED_GREEN;
        } else if(value < AMBIANTE) {
            PORTB = LED_GREEN;
            _delay_ms(DELAY_AMBRE);
            PORTB = LED_RED;
            _delay_ms(DELAY_AMBRE);
        } else {
            PORTB = LED_RED;
        }
    }
}
