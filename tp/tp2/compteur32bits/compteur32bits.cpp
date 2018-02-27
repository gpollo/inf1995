#include <avr/io.h> 

int main() {
    DDRA = 0xff;
    DDRB = 0xff;
    DDRC = 0xff;
    DDRD = 0xff;

    unsigned long compteur=0;

    for(;;) {
        compteur++;

        PORTD = compteur;
        PORTC = compteur >>  8;
        PORTB = compteur >> 16;
        PORTA = compteur >> 24;
    }

    return 0; 
}

