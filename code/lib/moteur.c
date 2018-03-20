#include <avr/io.h>
#include <moteur.h>
#include <util/delay.h>

/* valeur par défaut du prescaler */
#ifndef MOTEUR_PRESCALER
    #define MOTEUR_PRESCALER 64
#endif

void motor_init() {
    /* On réinitialise les registres impliqués */
    TCCR0A = 0;
    TCCR0B = 0;

    /* On s'assure que les roues sont arrêtées */
    moteur_arreter();

    /* clear au match du compteur et set au bas du compteur*/
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

    /* on configure les pins de PWM sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);
    
    /* par défaut, on met ces pins à bas */
    DIR0A_PIN &=~ _BV(DIR0A_BIT);
    DIR0B_PIN &=~ _BV(DIR0B_BIT);
}

void moteur_avancer(uint8_t speed_ratio) {
    /* On arrête les roues */
    moteur_arreter();

    /* On active la vitesse */
    uint8_t speed = (speed_ratio*255/100);
    OCR0A = speed;
    OCR0B = speed;

    /* On met les directions vers l'avant */
    DIR0A_PIN &=~ _BV(DIR0A_BIT);
    DIR0B_PIN &=~ _BV(DIR0B_BIT);

    /* On configure les pins du pwm en sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);
}

void moteur_reculer(uint8_t speed_ratio) {
    /* On arrête les roues */
    moteur_arreter();

    /* On active la vitesse */
    uint8_t speed = (speed_ratio*255/100);
    OCR0A = speed;
    OCR0B = speed;

    /* On met les directions vers l'arriere */
    DIR0A_PIN |= _BV(DIR0A_BIT);
    DIR0B_PIN |= _BV(DIR0B_BIT);

    /* On règle les pins du PWM en sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);
}

void moteur_arreter() {
    /* On configure les pins du PWM en sortie */
    OC0A_DDR &=~ _BV(OC0A_BIT);
    OC0B_DDR &=~ _BV(OC0B_BIT);
}

void moteur_tourner_droite() {
     /* On arrête les roues */
    moteur_arreter();

    /* On active la vitesse pour roue de gauche seulement */
    uint8_t speed = (ROTATION_SPEED*255/100);
    OCR0A = 0;
    OCR0B = speed;

    /* On met les directions vers l'avant */
    DIR0A_PIN &=~ _BV(DIR0A_BIT);
    DIR0B_PIN &=~ _BV(DIR0B_BIT);

    /* On configure les pins du pwm en sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);

    _delay_ms(500);
    moteur_arreter();
}

void moteur_tourner_gauche() {
     /* On arrête les roues */
    moteur_arreter();

    /* On active la vitesse pour roue de droite seulement */
    uint8_t speed = (ROTATION_SPEED*255/100);
    OCR0A = speed;
    OCR0B = 0;

    /* On met les directions vers l'avant */
    DIR0A_PIN &=~ _BV(DIR0A_BIT);
    DIR0B_PIN &=~ _BV(DIR0B_BIT);

    /* On configure les pins du pwm en sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);

    _delay_ms(500);
    moteur_arreter();
}
