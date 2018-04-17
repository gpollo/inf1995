#include <avr/io.h>
#include <moteur.h>
#include <util/delay.h>
#include <sensor.h>
#include <utils.h>
#include <uart.h>

#define DISTANCE_MINIMALE_CROISSIERE 200
#define DELAY_ROTATION90 950
#define DELAY_TOURNANT180 3000
#define DELAY_FIN180 1000

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
    SPEED_DROITE_DDR |= _BV(SPEED_DROITE_BIT);
    SPEED_GAUCHE_DDR |= _BV(SPEED_GAUCHE_BIT);
    DIR_DROITE_DDR |= _BV(DIR_DROITE_BIT);
    DIR_GAUCHE_DDR |= _BV(DIR_GAUCHE_BIT);
}

void moteur_avancer(uint8_t speed) {
    /* on met les directions vers l'avant */
    SET_DIRECTION_AVANCER(DROITE);
    SET_DIRECTION_AVANCER(GAUCHE);

    /* on active la vitesse */
    SET_SPEED(DROITE, speed-20);
    SET_SPEED(GAUCHE, speed);
}

void moteur_reculer(uint8_t speed) {
    /* on met les directions vers l'arriere */
    SET_DIRECTION_RECULER(DROITE);
    SET_DIRECTION_RECULER(GAUCHE);

    /* on active la vitesse */
    SET_SPEED(DROITE, speed);
    SET_SPEED(GAUCHE, speed);
}

void moteur_arreter() {
    /* on configure les pins du PWM en sortie */
    SET_SPEED(DROITE, 0);
    SET_SPEED(GAUCHE, 0);
}

void moteur_controler_droite(int8_t vitesse) {
    if(vitesse < 0) {
        /* on met la roue vers l'arrière */
        SET_DIRECTION_RECULER(DROITE);

        /* on rend la valeur positive */
        vitesse = -vitesse;
    } else {
        /* on met la roue vers l'avant */
        SET_DIRECTION_AVANCER(DROITE);
    }

    /* on ajuste la vitesse de la roue droite */
    SET_SPEED(DROITE, (vitesse * 255)/100);
}

void moteur_controler_gauche(int8_t vitesse) {
    if(vitesse < 0) {
        /* on met la roue vers l'arrière */
        SET_DIRECTION_RECULER(GAUCHE);

        /* on rend la valeur positive */
        vitesse = -vitesse;
    } else {
        /* on met la roue vers l'avant */
        SET_DIRECTION_AVANCER(GAUCHE);
    }

    /* on ajuste la vitesse de la roue gauche */
    SET_SPEED(GAUCHE, (vitesse * 255)/100);
}

void moteur_tourner_droite() {
    /* on tourne surplace */
    SET_DIRECTION_AVANCER(DROITE);
    SET_DIRECTION_RECULER(GAUCHE);

    /* on active la vitesse pour roue de gauche seulement */
    SET_SPEED(DROITE, ROTATION_SPEED+50);
    SET_SPEED(GAUCHE, ROTATION_SPEED+50);

    /* on attend un certain délais avant d'arrêter les moteurs */
    _delay_ms(DELAY_ROTATION90);
    moteur_arreter();
}

void moteur_tourner_gauche() {
    /* on met les directions vers l'avant */
    SET_DIRECTION_RECULER(DROITE);
    SET_DIRECTION_AVANCER(GAUCHE);

    /* on active la vitesse pour roue de droite seulement */
    SET_SPEED(DROITE, ROTATION_SPEED+50);
    SET_SPEED(GAUCHE, ROTATION_SPEED+50);

    /* on attend un certain délais avant d'arrêter les moteurs */
    _delay_ms(DELAY_ROTATION90);
    moteur_arreter();
}

void moteur_tourner_surplace(enum direction direction) {
    /* on configure la direction des moteurs */
    switch(direction) {
    case GAUCHE:
        /* on tourne à gauche */
        SET_DIRECTION_RECULER(GAUCHE);
        SET_DIRECTION_AVANCER(DROITE);
        break;
    case DROITE:
        /* on tourne à droite */
        SET_DIRECTION_AVANCER(GAUCHE);
        SET_DIRECTION_RECULER(DROITE);
        break;
    default:
        return;
    }

    /* on ajuste la force des moteurs  */
    SET_SPEED(GAUCHE, ROTATION_SPEED);
    SET_SPEED(DROITE, ROTATION_SPEED);
}

void moteur_tourner(enum direction direction) {
    struct moteurs moteurs;

    /* on configure la direction des moteurs */
    switch(direction) {
    case DROITE:
        /* seulement la roue de gauche bouge */
        moteurs.gauche.speed = ROTATION_SPEED+70;
        moteurs.droit.speed  = ROTATION_SPEED+20;
        break;
    case GAUCHE:
        /* seulement la roue de droite bouge */
        moteurs.gauche.speed = ROTATION_SPEED+20;
        moteurs.droit.speed  = ROTATION_SPEED+70;
        break;
    default:
        return;
    }

    /* les deux roues avancent */
    moteurs.gauche.avancer = TRUE;
    moteurs.droit.avancer  = TRUE;

    /* on ajuste les roues */
    moteur_config(&moteurs);
}

void moteur_tourner180(enum direction direction) {
    /* Pour tourner vers la gauche ou droite de 180 degrés */
    if(direction == GAUCHE) {
        moteur_tourner_droite();
        moteur_tourner_droite();
    } else if(direction == DROITE){
        moteur_tourner_gauche();
        moteur_tourner_gauche();
    }
}

void moteur_config(struct moteurs* moteurs) {
    /* on set la direction de la roue droite */
    if(moteurs->droit.avancer) {
        SET_DIRECTION_AVANCER(DROITE);
    } else {
        SET_DIRECTION_RECULER(DROITE);
    }

    /* on set la direction de la roue gauche */
    if(moteurs->gauche.avancer) {
        SET_DIRECTION_AVANCER(GAUCHE);
    } else {
        SET_DIRECTION_RECULER(GAUCHE);
    }

    /* on set la vitesse des deux roues */
    SET_SPEED(DROITE, moteurs->droit.speed-20);
    SET_SPEED(GAUCHE, moteurs->gauche.speed);
}
