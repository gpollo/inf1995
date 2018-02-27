#define F_CPU 8000000UL

#include <avr/io.h> 
#include <util/delay.h>
#define DELAY 10
#define BUTTON_PRESSED  0x04
#define BUTTON_RELEASED 0x00
#define LED_ON  0x1
#define LED_OFF 0x2

int main()
{
    DDRB = 0xff;
    DDRD = 0x00;

    uint8_t state = BUTTON_RELEASED;

    for(;;) {
        uint8_t value = PIND & 0x04;
        if(value == state)
            continue;

        state = value;
        _delay_ms(DELAY);

        if(state == BUTTON_PRESSED)
            PORTB = LED_ON;
        else if(state == BUTTON_RELEASED)
            PORTB = LED_OFF; 
    }

    return 0; 
}

