#include <avr/io.h>
#include <motor.h>

/* valeur par défaut du prescaler */
#ifndef PRESCALER
    #define PRESCALER 1
#endif

void motor_init() {
    /* on reset la configuration du compteur */
    TCCR0A = 0;

    /* clear au match du compteur et set au bas du compteur*/
    TCCR0A |= _BV(COM0A1);
    TCCR0A |= _BV(COM0B1);

    /* on configure le compteur en fast PWM */
    TCCR0A |= _BV(WGM01) | _BV(WGM00);

    /* on configure le prescaler des moteurs */
#if   PRESCALER == 1
    TCCR0B = _BV(CS00);
#elif PRESCALER == 8
    TCCR0B = _BV(CS01);
#elif PRESCALER == 64
    TCCR0B = _BV(CS01) | _BV(CS00);
#elif PRESCALER == 256
    TCCR0B = _BV(CS02);
#elif PRESCALER == 1024
    TCCR0B = _BV(CS02) | _BV(CS00);
#else
    #error "Invalid prescaled value"
#endif

    /* on configure la pin du PWM sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);
}
