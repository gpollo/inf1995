#include <avr/io.h>
#include <moteur.h>

/* valeur par défaut du prescaler */
#ifndef MOTEUR_PRESCALER
    #define MOTEUR_PRESCALER 64
#endif

void moteur_init() {
    /* on réinitialise les registres impliqués */
    TCCR0A = 0;
    TCCR0B = 0;

    /* on s'assure que les roues sont arrêtées */
    moteur_arreter();

    /* clear au match du compteur et set au bas du compteur */
    TCCR0A = _BV(COM0A1) | _BV(COM0B1);

    /* on configure le compteur en fast PWM */
    TCCR0A |= _BV(WGM01) | _BV(WGM00);

    /* on configure le prescaler des moteurs */
#if   MOTEUR_PRESCALER == 1
    TCCR0B = _BV(CS00);
#elif MOTEUR_PRESCALER == 8
    TCCR0B = _BV(CS01);
#elif MOTEUR_PRESCALER == 64
    TCCR0B = _BV(CS01) | _BV(CS00);
#elif MOTEUR_PRESCALER == 256
    TCCR0B = _BV(CS02);
#elif MOTEUR_PRESCALER == 1024
    TCCR0B = _BV(CS02) | _BV(CS00);
#else
    #error "Invalid prescaled value"
#endif

    /* on configure les pins de direction en sortie */
    DIR0A_DDR |= _BV(DIR0A_BIT);
    DIR0B_DDR |= _BV(DIR0B_BIT);

    /* par défaut, on met ces pins à bas */
    DIR0A_PIN &= ~_BV(DIR0A_BIT);
    DIR0B_PIN &= ~_BV(DIR0B_BIT);
}

void moteur_avancer(uint8_t speed) {
    /* on arrête les roues */
    moteur_arreter();

    /* on active la vitesse */
    OCR0A = speed;
    OCR0B = speed;

    /* on met les directions vers l'avant */
    DIR0A_PIN &= ~_BV(DIR0A_BIT);
    DIR0B_PIN &= ~_BV(DIR0B_BIT);

    /* on configure les pins du PWM en sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);
}

void moteur_reculer(uint8_t speed) {
    /* on arrête les roues */
    moteur_arreter();

    /* on active la vitesse */
    OCR0A = speed;
    OCR0B = speed;

    /* on met les directions vers l'avant */
    DIR0A_PIN |= _BV(DIR0A_BIT);
    DIR0B_PIN |= _BV(DIR0B_BIT);

    /* on configure les pins du PWM en sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);
}

void moteur_arreter() {
    /* on configure les pins du PWM en sortie */
    OC0A_DDR &= ~_BV(OC0A_BIT);
    OC0B_DDR &= ~_BV(OC0B_BIT);
}
