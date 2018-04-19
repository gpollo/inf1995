#include <stdlib.h>
#include <stdint.h>
#include <timer.h>
#include <sensor.h>
#include <uart.h>
#include <moteur.h>
#include <utils.h>

#include "robot.h"
#include "obstacle.h"
#include "state.h"

#define TIMEOUT_ROTATION45_GAUCHE 460
#define TIMEOUT_ROTATION45_DROITE 370

/**
 * Cette fonction actualise l'état du robot. Elle est appelée par un timer.
 *
 * @param data Un pointeur vers le robot.
 */
void update_state(void* data);

/** Cette structure configure le callback pour le timer du state. */
struct callback state_timer = {
    .func = &update_state,
    .time = DELAY,
    .repeat = 1,
    .data = NULL,
};

void state_start_timer(struct robot* robot) {
    /* on envoie le robot au callback */
    state_timer.data = (void*) robot;

    /* on active le callback */
    timer_start(&CALLBACK_IGNORE, &state_timer);
}

#define TRANSITION_ROTATION45(robot, direction, distance) { \
    (robot)->distance = (distance);                         \
    (robot)->timeout = TIMEOUT_ROTATION45_##direction;      \
    (robot)->state = ROTATION45_##direction;                \
}

#define TRANSITION_CHANGER_MUR(robot, direction) {         \
    (robot)->timeout = GET_TRAVEL_TIME((robot)->distance); \
    (robot)->state = CHANGER_MUR_##direction;              \
}

#define TRANSITION_ROTATION45_UNDO(robot, direction) { \
    (robot)->timeout = TIMEOUT_ROTATION45_##direction; \
    (robot)->state = ROTATION45_UNDO_##direction;      \
}

void update_state(void* data) {
    /* on obtient le robot */
    struct robot* robot = (struct robot*) data;

    /* on obtient les capteurs */
    struct capteurs* capteurs = &(robot->capteurs);

    /* on lit les capteurs */
    sensor_read(capteurs);
    sensor_get_value(&(capteurs->gauche));
    sensor_get_value(&(capteurs->droit));

    /* on regarde s'il y a un obstacle */
    enum obstacle obstacle = detect(robot);

    /* on change d'état si possible */
    switch(robot->state) {
    case RESET:
        /* si on ne détecte rien, on reste à cet état */
        if((!capteurs->gauche.capting) && (!capteurs->droit.capting)) {
            robot->state = RESET;
            break;
        }

        /* on ne détecte rien à gauche, on va à droite */ 
        if(!capteurs->gauche.capting) {
            robot->state = AVANCER_DROITE;
            break;
        }

        /* on ne détecte rien à droite, on va à gauche */ 
        if(!capteurs->droit.capting) {
            robot->state = AVANCER_GAUCHE;
            break;
        }

        /* si on détecte de chaque coté, on va vers le plus près */
        if(capteurs->gauche.value < capteurs->droit.value) {
            robot->state = AVANCER_GAUCHE;
        } else {
            robot->state = AVANCER_DROITE;
        }
        break;

    case AVANCER_GAUCHE:
        /* on suit le mur gauche */
        robot->mur = GAUCHE;

        /* on s'ajuste par rapport au mur gauche */
        robot_ajustement(robot);

        /* on regarde si on capte quelque chose à droite */
        if(capteurs->droit.capting) {
            robot->state = VERIFIER_DROITE;
            break;
        }
        break;

    case AVANCER_DROITE:
        /* on suit le mur droite */
        robot->mur = DROITE;

        /* on s'ajuste par rapport au mur droite */
        robot_ajustement(robot);

        /* on regarde si on capte quelque chose à gauche */
        if(capteurs->gauche.capting) {
            robot->state = VERIFIER_GAUCHE;
            break;
        }
        break;

    case AVANCER_GAUCHE_ATTENDRE:
        /* on suit le mur gauche */
        robot->mur = GAUCHE;

        /* on s'ajuste par rapport au mur gauche */
        robot_ajustement(robot);

        /* le robot peut changer de bord si on ne capte rien à droite */
        if(!capteurs->droit.capting) {
            robot->state = AVANCER_GAUCHE;
            break;
        }
        break;

    case AVANCER_DROITE_ATTENDRE:
        /* on suit le mur droite */
        robot->mur = DROITE;

        /* on s'ajuste par rapport au mur droite */
        robot_ajustement(robot);

        /* le robot peut changer de bord si on ne capte rien à gauche */
        if(!capteurs->gauche.capting) {
            robot->state = AVANCER_DROITE;
            break;
        }
        break;

    case VERIFIER_GAUCHE:
        /* on suit le mur droite */
        robot->mur = DROITE;

        /* on s'ajuste par rapport au mur gauche */
        robot_ajustement(robot);

        /* on ignore les poteaux */
        if(obstacle == POTEAU) {
            robot->state = AVANCER_DROITE;
            break;
        }

        /* on change de mur si possible */
        if(obstacle == MUR) {
            uint16_t distance = capteurs->gauche.value;
            if(distance > 250) {
                TRANSITION_ROTATION45(robot, GAUCHE, distance);
            } else {
                robot->state = AVANCER_GAUCHE_ATTENDRE;
            }
            break;
        }
        break;

    case VERIFIER_DROITE:
        /* on suit le mur gauche */
        robot->mur = GAUCHE;

        /* on s'ajuste par rapport au mur droite */
        robot_ajustement(robot);

        /* on ignore les poteaux */
        if(obstacle == POTEAU) {
            robot->state = AVANCER_GAUCHE;
            break;
        }

        /* on change de mur si possible */
        if(obstacle == MUR) {
            uint16_t distance = capteurs->droit.value;
            if(distance > 250) {
                TRANSITION_ROTATION45(robot, DROITE, distance);
            } else {
                robot->state = AVANCER_DROITE_ATTENDRE;
            }
            break;
        }
        break;

    case ROTATION45_GAUCHE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on change de mur */
        if(robot->timeout < 0) {
            TRANSITION_CHANGER_MUR(robot, GAUCHE);
            break;
        }

        /* on active les roues en rotation */
        moteur_tourner_surplace(GAUCHE);
        break;

    case ROTATION45_DROITE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on change de mur */
        if(robot->timeout < 0) {
            TRANSITION_CHANGER_MUR(robot, DROITE);
            break;
        }

        /* on active les roues en rotation */
        moteur_tourner_surplace(DROITE);
        break;

    case CHANGER_MUR_GAUCHE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on va vers le mur gauche */
        robot->mur = GAUCHE;

        /* on avance */
        moteur_avancer(128);

        /* on arrête d'avancer lorsque le timeout est finit */
        if((robot->timeout < 0) || ((90 < capteurs->gauche.value) && (capteurs->gauche.value < 140))) {
            TRANSITION_ROTATION45_UNDO(robot, GAUCHE);
            break;
        }
        break;

    case CHANGER_MUR_DROITE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on va vers le mur droite */
        robot->mur = DROITE;

        /* on avance */
        moteur_avancer(128);

        /* on arrête d'avancer lorsque le timeout est finit */
        if((robot->timeout < 0) || ((90 < capteurs->droit.value) && (capteurs->droit.value < 140))) {
            TRANSITION_ROTATION45_UNDO(robot, DROITE);
            break;
        }
        break;

    case ROTATION45_UNDO_GAUCHE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on change de mur */
        if(robot->timeout < 0) {
            robot->next = AVANCER_GAUCHE_ATTENDRE;
            robot->state = WAIT;
            robot->wait = 1000;
            break;
        }

        /* on active les roues en rotation */
        moteur_tourner_surplace(DROITE);
        break;

    case ROTATION45_UNDO_DROITE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on change de mur */
        if(robot->timeout < 0) {
            robot->next = AVANCER_DROITE_ATTENDRE;
            robot->state = WAIT;
            robot->wait = 1000;
            break;
        }

        /* on active les roues en rotation */
        moteur_tourner_surplace(GAUCHE);
        break;

    case WAIT:
        /* on actualise le timeout */
        robot->wait -= DELAY;

        /* on arrête les moteurs dans cet état */
        moteur_arreter();

        /* on change d'état à la fin du timeout */
        if(robot->wait < 0) {
            robot->state = robot->next;
            break;
        }
        break;

    default:
        break;
    }
}
