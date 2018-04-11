#include <avr/io.h>
#include <moteur.h>
#include <util/delay.h>
#include <sensor.h>
#include <utils.h>
#include <uart.h>

#define DISTANCE_SOUHAITE 150
#define DISTANCE_MINIMALE_CROISSIERE 200
#define CORRECTION_DOUCE 2
#define DELAY_ROTATION90 1750
#define DELAY_TOURNANT180 3000
#define DELAY_FIN180 1000

/* vitesse exprimer en mm par sec */
#define VITESSE_50PWM 70

/* valeur par défaut du prescaler */
#ifndef MOTEUR_PRESCALER
    #define MOTEUR_PRESCALER 64
#endif

/* vitesse de rotation par défaut */
#ifndef ROTATION_SPEED
    #define ROTATION_SPEED 128
#endif

/* vitesse maximale lors de l'ajustement à un mur */
#ifndef MAX_SPEED
    #define MAX_SPEED 200
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
    SET_SPEED(DROITE, speed);
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

void moteur_controller_gauche(int8_t vitesse) {
    if(vitesse < SPEED_STATIONARY) {
        /* On met les directions vers l'arrière */
        PORTB |= (1<<2);
        vitesse = -vitesse;
    } else {
        /* On met les directions vers l'avant */
        PORTB &=~ (1<<2);
    }

    /* On active la vitesse pour roue de droite seulement */
    OCR0A = SPEED_ROTATION(vitesse);
}

void moteur_controller_droite(int8_t vitesse) {
    if(vitesse < SPEED_STATIONARY) {
        /* On met les directions vers l'arrière */
        PORTB |= (1<<5);
        vitesse = -vitesse;
    } else {
        /* On met les directions vers l'avant */
        PORTB &=~ (1<<5);
    }

    /* On active la vitesse pour roue de droite seulement */
    OCR0B = SPEED_ROTATION(vitesse);
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

void moteur_tourner180(uint8_t direction) {
    /* Pour tourner vers la gauche */
    if(direction == 0) {
        moteur_tourner_gauche();
        moteur_avancer(ROTATION_SPEED);
        _delay_ms(DELAY_TOURNANT180);
        moteur_tourner_gauche();
        moteur_avancer(ROTATION_SPEED);
        _delay_ms(DELAY_FIN180);
    } else {
        moteur_tourner_droite();
        moteur_avancer(ROTATION_SPEED);
        _delay_ms(DELAY_TOURNANT180);
        moteur_tourner_droite();
        moteur_avancer(ROTATION_SPEED);
        _delay_ms(DELAY_FIN180);
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
    SET_SPEED(DROITE, moteurs->droit.speed);
    SET_SPEED(GAUCHE, moteurs->gauche.speed);
}

void moteur_ajustement(struct capteurs* capteurs, uint8_t direction) {
    /* on obtient la distance du capteurs */
    int16_t distance;
    if(direction == 0) {
        distance = capteurs->gauche.value;
    } else {
        distance = capteurs->droit.value;
    }

    /* on calcul l'erreur */
    int16_t erreur = DISTANCE_SOUHAITE - distance;

    /* le facteur de correction */
    int16_t correction = CORRECTION_DOUCE;

    /* on calcule les vitesses à envoyer aux moteurs */
    struct moteurs moteurs;
    if(direction == 0) {
        if(erreur > 0) {
            /* on s'éloigne rapidement du mur */
            moteurs.gauche.speed = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.gauche.avancer = 0;
            moteurs.droit.avancer  = 1;
        } else {
            uint16_t ajustement = ROTATION_SPEED - erreur/correction;

            /* on s'approche tranquillement du mur */
            moteurs.gauche.speed = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ajustement, MAX_SPEED);
            moteurs.gauche.avancer = 1;
            moteurs.droit.avancer  = 1;
        }
    } else {
        if(erreur > 0) {
            /* on s'éloigne rapidement du mur */
            moteurs.gauche.speed = ROTATION_SPEED;
            moteurs.droit.speed  = ROTATION_SPEED;
            moteurs.gauche.avancer = 1;
            moteurs.droit.avancer  = 0;
        } else {
            uint16_t ajustement = ROTATION_SPEED - erreur/correction;

            /* on s'approche tranquillement du mur */
            moteurs.gauche.speed = LIMIT(ajustement, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.gauche.avancer = 1;
            moteurs.droit.avancer  = 1;
        }
    }

    /* on ajuste les roues */
    moteur_config(&moteurs);

    /* information pour debug */
    uart_debug("%d %d -- %i %i -- %i %i / %i %i\n\r",
        erreur,
        distance,
        capteurs->gauche,
        capteurs->droite,
        speed_gauche,
        speed_droite,
        moteurs.gauche.speed,
        moteurs.droit.speed
    );
}

void changement_coter(struct capteurs* capteurs, uint8_t direction) {
    /* Chercher les valeurs des capteurs*/
    uint16_t dist_gauche = capteurs->gauche.value;
    uint16_t dist_droite = capteurs->droit.value;
    
    /* On commencer par savoir qu'elle direction est présentement suivi */
    if(direction == 0) {
        if(dist_droite < DISTANCE_MINIMALE_CROISSIERE) {return;}
        /* Initialise le changement en orientant le robot */
        moteur_tourner_droite();

        /* S'approche du nouveau mur suivi */
        uint16_t temps_croissiere1 = temps_croissiere(dist_droite);
        moteur_avancer(ROTATION_SPEED);
        for(uint16_t i = 0; i< temps_croissiere1;i++) {       
            _delay_ms(1000);
        }

        /* Puis on réoriente le robot pour continuer le suivi du nouveau mur */
        moteur_tourner_gauche();
    } else {
        if(dist_gauche < DISTANCE_MINIMALE_CROISSIERE) {return;}
        /* Initialise le changement en orientant le robot */
        moteur_tourner_gauche();

        /* S'approche du nouveau mur suivi */
        uint16_t temps_croissiere1 = temps_croissiere(dist_gauche);
        moteur_avancer(ROTATION_SPEED);
        for(uint16_t i = 0; i< temps_croissiere1;i++) {
            _delay_ms(1000);
        }
        
        /* Puis on réoriente le robot pour continuer le suivi du nouveau mur */
        moteur_tourner_droite();
    }
}

uint16_t temps_croissiere(uint16_t distance_a_faire) {
    /* 
     * Vitesse du robot à 50 PWM delay en milisecond donc multiplier par 1000 et
     * divise. La distance à faire est en mm et la vitesse est en mm/s donc il
     * reste des secondes.
     **/
    return  (distance_a_faire-DISTANCE_SOUHAITE)/VITESSE_50PWM;
}
