#include <avr/io.h>
#include <moteur.h>
#include <util/delay.h>
#include <sensor.h>

#define DISTANCE_SOUHAITE 150
#define CORRECTION_DOUCE 2
#define DELAY_ROTATION90 500


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
    PORTB |= (1<<2);
    PORTB &=~ (1<<5);

    /* On active la vitesse pour roue de gauche seulement */
    OCR0A = ROTATION_SPEED;
    OCR0B = ROTATION_SPEED;

    _delay_ms(DELAY_ROTATION90);
    moteur_arreter();
}

void moteur_tourner_gauche() {
    /* On met les directions vers l'avant */
    PORTB &=~ (1<<2);
    PORTB |= (1<<5);

    /* On active la vitesse pour roue de droite seulement */
    OCR0A = ROTATION_SPEED;
    OCR0B = ROTATION_SPEED;

    _delay_ms(DELAY_ROTATION90);
    moteur_arreter();
}

#include <uart.h>

void moteur_ajustement(struct capteurs* capteurs, uint8_t direction) {

    /* Calcul de la vitesse selon distance captés */
    /* Chercher les valeurs des capteurs*/
    uint16_t dist_gauche = sensor_get_distance(capteurs->gauche);
    uint16_t dist_droite = sensor_get_distance(capteurs->droite);

    /*********************************************************************** 
     * Calcul du facteur de correction selon ces distances 
     * Initialise les paramètres
     * Définie la direction Gauche = 0  et Droite =1
     * Multiplier par un facteur de correction pour corriger la trajectoire
     ***********************************************************************/
    int16_t erreur;
    uint16_t speed_gauche;
    uint16_t speed_droite;
    int16_t correction = CORRECTION_DOUCE;

    if(direction == 0) {
        erreur = DISTANCE_SOUHAITE - dist_gauche;
        if(erreur > 0) {
            speed_gauche = ROTATION_SPEED;
            speed_droite = ROTATION_SPEED;
            /* Gauche vers l'avant et droite vers l'arriere */
            PORTB &=~ (1<<2);
            PORTB |= (1<<5);
        } else {
            speed_gauche = ROTATION_SPEED;
            speed_droite = ROTATION_SPEED - erreur/correction;
            /* On met les directions des deux roues vers l'avant */
            PORTB &=~ (1<<2);
            PORTB &=~ (1<<5);
        }
    } else {
        erreur = DISTANCE_SOUHAITE - dist_droite;
        if(erreur > 0) {
            speed_gauche = ROTATION_SPEED;
            speed_droite = ROTATION_SPEED;
            /* Gauche recule et droite avant */
            PORTB |= (1<<2);
            PORTB &=~ (1<<5);
        } else {
            speed_gauche = ROTATION_SPEED - erreur/correction;
            speed_droite = ROTATION_SPEED;
            /* On met les directions des deux roues vers l'avant */
            PORTB &=~ (1<<2);
            PORTB &=~ (1<<5);
        }
    }

    /* On définie la vitesse de chaque roues valeur allant de 90 à 180 */
    OCR0A = (speed_droite > 200) ? 200 : speed_droite;
    OCR0B = (speed_gauche > 200) ? 200 : speed_gauche;

    uart_printf("%d -- %i %i -- %i %i\n\t", erreur, speed_droite, speed_gauche, OCR0A, OCR0B);
}

void changement_coter(uint8_t direction) {
    /* On commencer par savoir qu'elle direction est présentement suivi */
    if(direction = 0) {
        /* Initialise le changement en orientant le robot */
        /* fait avancer */
       

    } else {
    }

}













