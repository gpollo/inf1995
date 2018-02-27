#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

void reset_timer() {
    TCCR0A = 0;
    TCCR0B = 0;   
}

void fast_pwm()
{
    reset_timer();

    // initialize timer0 in PWM mode
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);

    TCCR0A |= (1 << WGM01) | (1 << WGM00);

    TCCR0B |= (1 << CS00) | (1 << CS01);
    
    // make sure to make OC0 pin (pin PB3 for atmega32) as output pin
    DDRB = 0xff;
}

void phase_correct_pwm()
{
    reset_timer();

    // initialize timer0 in PWM mode
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);

    TCCR0A |= (1 << WGM00);

    TCCR0B |= (1 << CS02);

    // make sure to make OC0 pin (pin PB3 for atmega32) as output pin
    DDRB = 0xff;
}

void test_speed()
{
    OCR0A = 0;
    _delay_ms(2000);

    OCR0A = 64;
    _delay_ms(2000);

    OCR0A = 128;
    _delay_ms(2000);

    OCR0A = 192;
    _delay_ms(2000);

    OCR0A = 255;
    _delay_ms(2000);
}

int main(void)
{
    while(1) {
        phase_correct_pwm();
        test_speed();

        fast_pwm();
        test_speed();
    }

    return 0;
}
