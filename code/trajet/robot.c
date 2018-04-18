#include <moteur.h>
#include <sensor.h>
#include <del.h>
#include <utils.h>

#include "robot.h"

/* le facteur de correction */
#ifndef CORRECTION
    #define CORRECTION 4
#endif

/* la vitesse maximale qu'on envoit aux roues */
#ifndef MAX_SPEED
    #define MAX_SPEED 220
#endif

/* l'incertitude de la distance pour ajuster la del */
#ifndef INCERTITUDE_DEL
    #define INCERTITUDE_DEL 20
#endif

int16_t robot_get_erreur(struct robot* robot) {
    /* on obtient les capteurs */
    struct capteurs* capteurs = (struct capteurs*) &(robot->capteurs);

    /* on obtient la distance du mur  */
    int16_t distance;
    switch(robot->mur) {
    case GAUCHE:
        distance = capteurs->gauche.value;
        break;
    case DROITE:
        distance = capteurs->droit.value;
        break;
    default:
        distance = 0;
        break;
    }

    /* on calcul l'erreur */
    return (DISTANCE_SOUHAITEE-distance);
}

void robot_ajust_del(int16_t erreur) {
    /* on prend la valeur absolue */
    erreur = (erreur < 0) ? (-erreur) : (erreur);

    /* on controle la del */
    if(erreur < INCERTITUDE_DEL) {
        del_green();
    } else {
        del_red();
    }
}

void robot_ajustement(struct robot* robot) {
    /* on obtient l'erreur */
    int16_t erreur = robot_get_erreur(robot);

    /* on obtient les capteurs */
    struct capteurs* capteurs = &(robot->capteurs);

    /* on ajuste la del */
    robot_ajust_del(erreur);

    /* on calcule les vitesses à envoyer aux moteurs */
    struct moteurs moteurs = {
        .gauche = {
            .speed = ROTATION_SPEED,
            .avancer = 1,
        },
        .droit = {
            .speed = ROTATION_SPEED,
            .avancer = 1,
        }
    };

    switch(robot->mur) {
    case GAUCHE:
        if(capteurs->gauche.value == 0) {
            /* on tente de recapter */
            moteurs.gauche.speed = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ROTATION_SPEED+60, MAX_SPEED);
            moteurs.gauche.avancer = 1;
            moteurs.droit.avancer  = 1;           
        } else if(erreur > 0) {
            /* on s'éloigne rapidement du mur */
            moteurs.gauche.speed = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.gauche.avancer = 1;
            moteurs.droit.avancer  = 0;
        } else {
            uint16_t ajustement = ROTATION_SPEED - erreur;//CORRECTION;

            /* on s'approche tranquillement du mur */
            moteurs.gauche.speed = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ajustement, MAX_SPEED);
            moteurs.gauche.avancer = 1;
            moteurs.droit.avancer  = 1;
        }
        break;
    case DROITE:
        if(capteurs->droit.value == 0) {
            /* on tente de recapter */
            moteurs.gauche.speed = LIMIT(ROTATION_SPEED+60, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.gauche.avancer = 1;
            moteurs.droit.avancer  = 1;
        } else if(erreur > 0) {
            /* on s'éloigne rapidement du mur */
            moteurs.gauche.speed = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.gauche.avancer = 0;
            moteurs.droit.avancer  = 1;
        } else {
            uint16_t ajustement = ROTATION_SPEED - erreur;//CORRECTION;

            /* on s'approche tranquillement du mur */
            moteurs.gauche.speed = LIMIT(ajustement, MAX_SPEED);
            moteurs.droit.speed  = LIMIT(ROTATION_SPEED, MAX_SPEED);
            moteurs.gauche.avancer = 1;
            moteurs.droit.avancer  = 1;
        }
        break;
    default:
        break;
    }

    /* on ajuste les roues */
    moteur_config(&moteurs);
}
