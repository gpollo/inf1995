#include <avr/io.h>
#include <moteur.h>
#include <util/delay.h>

/* valeur par défaut du prescaler */
#ifndef MOTEUR_PRESCALER
    #define MOTEUR_PRESCALER 64
#endif

void moteur_init() {
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
    DDRB = 0xff;    
}

void moteur_avancer(uint8_t speed) {
    /* On met les directions vers l'avant */
    PORTB &=~ (1<<2);
    PORTB &=~ (1<<5);

    /* On active la vitesse */
    OCR0A = speed;
    OCR0B = speed;
}

void moteur_reculer(uint8_t speed) {
    /* On met les directions vers l'arriere */
    PORTB |= (1<<2);
    PORTB |= (1<<5);

    /* On active la vitesse */
    OCR0A = speed;
    OCR0B = speed;
}

void moteur_arreter() {
    /* On configure les pins du PWM en sortie */
    OCR0A = 0;
    OCR0B = 0;
}

void moteur_tourner_droite() {
    /* On met les directions vers l'avant */
    PORTB &=~ (1<<2);
    PORTB &=~ (1<<5);

    /* On active la vitesse pour roue de gauche seulement */
    OCR0A = 0;
    OCR0B = ROTATION_SPEED;

    _delay_ms(2300);
    moteur_arreter();
}

void moteur_tourner_gauche() {
    /* On met les directions vers l'avant */
    PORTB &=~ (1<<2);
    PORTB &=~ (1<<5);

    /* On active la vitesse pour roue de droite seulement */
    OCR0A = ROTATION_SPEED;
    OCR0B = 0;

    _delay_ms(2300);
    moteur_arreter();
}

void moteur_controller_gauche(int8_t vitesse) {
    if(vitesse < 0) {
        /* On met les directions vers l'arrière */
        PORTB |= (1<<2);
        vitesse = -vitesse;
    } else {
        /* On met les directions vers l'avant */
        PORTB &=~ (1<<2);
    }

    /* On active la vitesse pour roue de droite seulement */
    OCR0A = (vitesse*255)/100;
}

void moteur_controller_droite(int8_t vitesse) {
    if(vitesse < 0) {
        /* On met les directions vers l'arrière */
        PORTB |= (1<<5);
        vitesse = -vitesse;
    } else {
        /* On met les directions vers l'avant */
        PORTB &=~ (1<<5);
    }

    /* On active la vitesse pour roue de droite seulement */
    OCR0B = (vitesse*255)/100;
}

