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

#define TIMEOUT_ROTATION45 900
#define CHANGER_MUR_STOP_DISTANCE 140

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
    (robot)->distance = distance;                           \
    (robot)->timeout = TIMEOUT_ROTATION45;                  \
    (robot)->state = ROTATION45_##direction;                \
}

#define TRANSITION_CHANGER_MUR(robot, direction) {        \
    (robot)->timeout = GET_TRAVEL_TIME((robot)->distance) \
    (robot)->state = CHANGER_MUR_##direction;             \
}

#define TRANSITION_ROTATION45_UNDO(robot, direction) { \
    (robot)->timeout = TIMEOUT_ROTATION45;             \
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

    /* on écrit l'état */
    uart_printf("%d ", robot->state);
/*
    uart_printf("%d %d\n\r",
        capteurs->gauche.value,
        capteurs->droit.value
    );
*/
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

        /* on regarde si on capte encore vers la gauche */
        if(!capteurs->gauche.capting) {
            //robot->state = TOURNER_GAUCHE;
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

        /* on regarde si on capte encore vers la droite */
        if(!capteurs->droit.capting) {
            //robot->state = TOURNER_DROITE;
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

        /* si on ne capte plus rien à gauche, on tourne */
        if(!capteurs->gauche.capting) {
            robot->state = TOURNER_GAUCHE;
            moteur_tourner(GAUCHE);
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

        /* si on ne capte plus rien à droite, on tourne */
        if(!capteurs->droit.capting) {
            robot->state = TOURNER_DROITE;
            moteur_tourner(DROITE);
            break;
        }
        break;

    case VERIFIER_GAUCHE:
        /* on suit le mur gauche */
        robot->mur = GAUCHE;

        /* on s'ajuste par rapport au mur gauche */
        robot_ajustement(robot);

        /* on ignore les poteaux */
        if(obstacle == POTEAU) {
            robot->state = AVANCER_DROITE;
            break;
        }

        /* on change de mur si possible */
        if(obstacle == MUR) {
            TRANSITION_ROTATION45(robot, GAUCHE, capteurs->gauche.value);
            break;
        }
        break;

    case VERIFIER_DROITE:
        /* on suit le mur droite */
        robot->mur = DROITE;

        /* on s'ajuste par rapport au mur droite */
        robot_ajustement(robot);

        /* on ignore les poteaux */
        if(obstacle == POTEAU) {
            robot->state = AVANCER_GAUCHE;
            break;
        }

        /* on change de mur si possible */
        if(obstacle == MUR) {
            TRANSITION_ROTATION45(robot, DROITE, capteurs->droit.value);
            break;
        }
        break;

    case VERIFIER_GAUCHE_ROTATION:
        /* on s'assure que le moteur est en rotation vers la gauche */
        moteur_tourner(GAUCHE);

        /* à un poteau, on continue de tourner */
        if(obstacle == POTEAU) {
            robot->state = TOURNER_DROITE;
            break;
        }

        /* à un mur, on change de coté */
        if(obstacle == MUR) {
            robot->state = CHANGER_MUR_GAUCHE;
            break;
        }
        break;

    case VERIFIER_DROITE_ROTATION:
        /* on s'assure que le moteur est en rotation vers la droite */
        moteur_tourner(DROITE);

        /* à un poteau, on continue de tourner */
        if(obstacle == POTEAU) {
            robot->state = TOURNER_GAUCHE;
            break;
        }

        /* à un mur, on change de coté */
        if(obstacle == MUR) {
            robot->state = CHANGER_MUR_DROITE;
            break;
        }
        break;

    case TOURNER_GAUCHE:
        /* on s'assure que le moteur est en rotation vers la gauche */
        moteur_tourner(GAUCHE);

        /* on regarde si on capte quelque chose vers la gauche */
        if(capteurs->gauche.capting) {
            robot->state = AVANCER_GAUCHE;
            break;
        }

        /* on vérifie ce qu'il y a à droite */
        if(capteurs->droit.capting) {
            robot->state = VERIFIER_DROITE_ROTATION;
            break;
        }
        break;

    case TOURNER_DROITE:
        /* on s'assure que le moteur est en rotation vers la droite */
        moteur_tourner(DROITE);

        /* on regarde si on capte quelque chose vers la droite */
        if(capteurs->droit.capting) {
            robot->state = AVANCER_DROITE;
            break;
        }

        /* on vérifie ce qu'il y a à droite */
        if(capteurs->gauche.capting) {
            robot->state = VERIFIER_GAUCHE_ROTATION;
            break;
        }
        break;

    case ROTATION45_GAUCHE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on change de mur */
        if(robot->timeout < 0) {
            robot->timeout
            robot->state = CHANGER_MUR_GAUCHE;
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
            robot->state = CHANGER_MUR_DROITE;
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

        /* on arrête d'avancer lorsque le timeout est finit */
        if(robot->timeout < 0) {
            TRANSITION_ROTATION45_UNDO(robot, GAUCHE);
            break;
        }

        /* on avance */
        moteur_avancer(128);
        break;

    case CHANGER_MUR_DROITE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on va vers le mur droite */
        robot->mur = DROITE;

        /* on arrête d'avancer lorsque le timeout est finit */
        if(robot->timeout < 0) {
            TRANSITION_ROTATION45_UNDO(robot, DROITE);
            break;
        }

        /* on avance */
        moteur_avancer(128);
        break;

    case ROTATION45_UNDO_GAUCHE:
        /* on actualise le timeout */
        robot->timeout -= DELAY;

        /* on change de mur */
        if(robot->timeout < 0) {
            robot->state = AVANCER_GAUCHE_ATTENDRE;
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
            robot->state = AVANCER_DROITE_ATTENDRE;
            break;
        }

        /* on active les roues en rotation */
        moteur_tourner_surplace(GAUCHE);
        break;
    }
}
