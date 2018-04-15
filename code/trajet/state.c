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
    uart_debug("%d ", robot->state);

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
        moteur_ajustement(capteurs, robot->mur);

        /* on regarde si on capte quelque chose à droite */
        if(capteurs->droit.capting) {
            robot->state = VERIFIER_DROITE;
            break;
        }

        /* on regarde si on capte encore vers la gauche */
        if(!capteurs->gauche.capting) {
            robot->state = TOURNER_GAUCHE;
            break;
        }
        break;

    case AVANCER_DROITE:
        /* on suit le mur droite */
        robot->mur = DROITE;

        /* on s'ajuste par rapport au mur droite */
        moteur_ajustement(capteurs, robot->mur);

        /* on regarde si on capte quelque chose à gauche */
        if(capteurs->gauche.capting) {
            robot->state = VERIFIER_GAUCHE;
            break;
        }

        /* on regarde si on capte encore vers la droite */
        if(!capteurs->droit.capting) {
            robot->state = TOURNER_DROITE;
            break;
        }
        break;

    case AVANCER_GAUCHE_ATTENDRE:
        /* on suit le mur gauche */
        robot->mur = GAUCHE;

        /* on s'ajuste par rapport au mur gauche */
        moteur_ajustement(capteurs, robot->mur);

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
        moteur_ajustement(capteurs, robot->mur);

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
        moteur_ajustement(capteurs, robot->mur);

        /* on ignore les poteaux */
        if(obstacle == POTEAU) {
            robot->state = AVANCER_DROITE;
            break;
        }

        /* on change de mur si possible */
        if(obstacle == MUR) {
            robot->state = CHANGER_MUR_GAUCHE;
            break;
        }

        /* on change d'état selon l'obstacle */
        CHECK_OBSTACLE(obstacle, robot, DROITE, GAUCHE);
        break;

    case VERIFIER_DROITE:
        /* on suit le mur droite */
        robot->mur = DROITE;

        /* on s'ajuste par rapport au mur droite */
        moteur_ajustement(capteurs, robot->mur);

        /* on ignore les poteaux */
        if(obstacle == POTEAU) {
            robot->state = AVANCER_GAUCHE;
            break;
        }

        /* on change de mur si possible */
        if(obstacle == MUR) {
            robot->state = CHANGER_MUR_DROITE;
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

    case CHANGER_MUR_GAUCHE:
        uart_printf("gauche\n\r");
        changement_coter(capteurs, 0);
        robot->state = AVANCER_GAUCHE_ATTENDRE;
        break;

    case CHANGER_MUR_DROITE:
        uart_printf("droit\n\r");
        changement_coter(capteurs, 1);
        robot->state = AVANCER_DROITE_ATTENDRE;
        break;
    }
}
