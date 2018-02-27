#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

void pwm_init()
{
    // initialize timer0 in PWM mode
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);

    TCCR0A |= (1 << WGM01) | (1 << WGM00);

    TCCR0B |= (1 << CS00);
    
    // make sure to make OC0 pin (pin PB3 for atmega32) as output pin
    DDRB = 0xff;
}

void change_brightness()
{

    uint8_t brightness;

        // increasing brightness
        for (brightness = 0; brightness < 255; ++brightness)
        {
            // set the brightness as duty cycle
            OCR0A = brightness;

            // delay so as to make the user "see" the change in brightness
            _delay_ms(12);
        }

        // decreasing brightness
        for (brightness = 255; brightness > 0; --brightness)
        {
            // set the brightness as duty cycle
            OCR0A = brightness;

            // delay so as to make the user "see" the change in brightness
            _delay_ms(12);
        }

        // increasing brightness
        for (brightness = 0; brightness < 255; ++brightness)
        {
            // set the brightness as duty cycle
            OCR0B = brightness;

            // delay so as to make the user "see" the change in brightness
            _delay_ms(12);
        }

        // decreasing brightness
        for (brightness = 255; brightness > 0; --brightness)
        {
            // set the brightness as duty cycle
            OCR0B = brightness;

            // delay so as to make the user "see" the change in brightness
            _delay_ms(12);
        }
}

int main(void)
{

    // initialize timer0 in PWM mode
    pwm_init();
    
    while(1) change_brightness();
}
