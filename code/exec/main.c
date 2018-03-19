#include <avr/io.h>
#include <moteur.h>
#include <util/delay.h>
#include <son.h>

int main(void) {
    moteur_init();
    son_init();

    DDRD |= _BV(4);
    PORTD |= _BV(4);
    son_depart();

    while(1) {
        for(uint8_t speed = 50; speed < 200; speed +=50) {
            moteur_avancer(speed);
            _delay_ms(2000);
            moteur_reculer(speed);
            _delay_ms(2000);
        }
    }
}
