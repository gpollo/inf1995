#include <avr/io.h>
#include <moteur.h>
#include <util/delay.h>
#include <sensor.h>
#include <utils.h>
#include <uart.h>

#define DISTANCE_SOUHAITE 150
#define CORRECTION_DOUCE 2
#define DELAY_ROTATION90 2500

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
    SET_SPEED(DROITE, speed);
    SET_SPEED(GAUCHE, speed);
}

void moteur_reculer(uint8_t speed) {
    /* On met les directions vers l'arriere */
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

void moteur_tourner_droite() {
    /* on tourne surplace */
    SET_DIRECTION_AVANCER(DROITE);
    SET_DIRECTION_RECULER(GAUCHE);

    /* on active la vitesse pour roue de gauche seulement */
    SET_SPEED(DROITE, ROTATION_SPEED);
    SET_SPEED(GAUCHE, ROTATION_SPEED);

    /* on attend un certain délais avant d'arrêter les moteurs */
    _delay_ms(DELAY_ROTATION90);
    moteur_arreter();
}

void moteur_tourner_gauche() {
    /* on met les directions vers l'avant */
    SET_DIRECTION_RECULER(DROITE);
    SET_DIRECTION_AVANCER(GAUCHE);

    /* on active la vitesse pour roue de droite seulement */
    SET_SPEED(DROITE, ROTATION_SPEED);
    SET_SPEED(GAUCHE, ROTATION_SPEED);

    /* on attend un certain délais avant d'arrêter les moteurs */
    _delay_ms(DELAY_ROTATION90);
    moteur_arreter();
}

void moteur_control(struct moteur_control* control) {
    /* on set la direction de la roue droite */
    if(control->droite_avancer) {
        SET_DIRECTION_AVANCER(DROITE);
    } else {
        SET_DIRECTION_RECULER(DROITE);
    }

    /* on set la direction de la roue gauche */
    if(control->gauche_avancer) {
        SET_DIRECTION_AVANCER(GAUCHE);
    } else {
        SET_DIRECTION_RECULER(GAUCHE);
    }

    /* on set la vitesse des deux roues */
    SET_SPEED(DROITE, control->droite_speed);
    SET_SPEED(GAUCHE, control->gauche_speed);
}

void moteur_ajustement(struct capteurs* capteurs, uint8_t direction) {
    /* on obtient la distance du capteurs */
    int16_t distance;
    if(direction == 0) {
        distance = sensor_get_distance(capteurs->gauche);
    } else {
        distance = sensor_get_distance(capteurs->droite);
    }

    /* on calcul l'erreur */
    int16_t erreur = DISTANCE_SOUHAITE - distance;

    /* le facteur de correction */
    int16_t correction = CORRECTION_DOUCE;

    uint16_t speed_gauche;
    uint16_t speed_droite;

    /* on calcule les vitesses à envoyer aux moteurs */
    struct moteur_control control;
    if(direction == 0) {
        if(erreur > 0) {
            /* on s'éloigne rapidement du mur */
            speed_gauche = ROTATION_SPEED;
            speed_droite = ROTATION_SPEED;
            control.gauche_avancer = 0;
            control.droite_avancer = 1;
        } else {
            /* on s'approche tranquillement du mur */
            speed_gauche = ROTATION_SPEED;
            speed_droite = ROTATION_SPEED - erreur/correction;
            control.gauche_avancer = 1;
            control.droite_avancer = 1;
        }
    } else {
        if(erreur > 0) {
            /* on s'éloigne rapidement du mur */
            speed_gauche = ROTATION_SPEED;
            speed_droite = ROTATION_SPEED;
            control.gauche_avancer = 1;
            control.droite_avancer = 0;
        } else {
            /* on s'approche tranquillement du mur */
            speed_gauche = ROTATION_SPEED - erreur/correction;
            speed_droite = ROTATION_SPEED;
            control.gauche_avancer = 1;
            control.droite_avancer = 1;
        }
    }

    /* on limite la vitesse des roues */
    LIMIT_VALUE(speed_gauche, 200);
    LIMIT_VALUE(speed_droite, 200);
    control.gauche_speed = speed_gauche;
    control.droite_speed = speed_droite;

    /* on ajuste les roues */
    moteur_control(&control);

    /* information pour debug */
    uart_debug("%d %d -- %i %i -- %i %i / %i %i\n\r",
        erreur,
        distance,
        capteurs->gauche,
        capteurs->droite,
        speed_gauche,
        speed_droite,
        control.gauche_speed,
        control.droite_speed
    );
}

void changement_coter(uint8_t direction) {
    /* On commencer par savoir qu'elle direction est présentement suivi */
    if(direction = 0) {
        /* Initialise le changement en orientant le robot */
        /* fait avancer */
       

    } else {
    }

}













