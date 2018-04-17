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

#define TIMEOUT_ROTATION45 550

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
    (robot)->timeout = TIMEOUT_ROTATION45;                  \
    (robot)->state = ROTATION45_##direction;                \
}

#define TRANSITION_CHANGER_MUR(robot, direction) {         \
    (robot)->timeout = GET_TRAVEL_TIME((robot)->distance); \
    (robot)->state = CHANGER_MUR_##direction;              \
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
            robot->state = TOURNER_GAUCHE;
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
            robot->state = TOURNER_DROITE;
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

    case VERIFIER_GAUCHE_ROTATION:
        /* on s'assure que le moteur est en rotation vers la droite */
        moteur_tourner(DROITE);

        /* à un poteau, on continue de tourner */
        if(obstacle == POTEAU) {
            robot->state = TOURNER_DROITE;
            break;
        }

        /* à un mur, on change de coté */
        if(obstacle == MUR) {
            robot->state = AVANCER_GAUCHE_ATTENDRE;
            break;
        }
        break;

    case VERIFIER_DROITE_ROTATION:
        /* on s'assure que le moteur est en rotation vers la gauche */
        moteur_tourner(GAUCHE);

        /* à un poteau, on continue de tourner */
        if(obstacle == POTEAU) {
            robot->state = TOURNER_GAUCHE;
            break;
        }

        /* à un mur, on change de coté */
        if(obstacle == MUR) {
            robot->state = AVANCER_DROITE_ATTENDRE;
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

        /* on arrête d'avancer lorsque le timeout est finit */
        if((robot->timeout < 0) || (capteurs->gauche.value < 250)) {
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
        if((robot->timeout < 0) || (capteurs->droit.value < 250)) {
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

    default:
        break;
    }
}

void test(void* data) {
    static void** states[RESET+1];
    static uint8_t configured = FALSE;

    /* on configure la look-up-table si ce n'est pas fait */
    if(!configured) {
        configured = TRUE;
        states[RESET                   ] = &&STATE_RESET;
        states[AVANCER_GAUCHE          ] = &&STATE_AVANCER_GAUCHE;
        states[AVANCER_DROITE          ] = &&STATE_AVANCER_DROITE;
        states[AVANCER_GAUCHE_ATTENDRE ] = &&STATE_AVANCER_GAUCHE_ATTENDRE;
        states[AVANCER_DROITE_ATTENDRE ] = &&STATE_AVANCER_DROITE_ATTENDRE;
        states[VERIFIER_GAUCHE         ] = &&STATE_VERIFIER_GAUCHE;
        states[VERIFIER_DROITE         ] = &&STATE_VERIFIER_DROITE;
        states[VERIFIER_GAUCHE_ROTATION] = &&STATE_VERIFIER_GAUCHE_ROTATION;
        states[VERIFIER_DROITE_ROTATION] = &&STATE_VERIFIER_DROITE_ROTATION;
        states[TOURNER_GAUCHE          ] = &&STATE_TOURNER_GAUCHE;
        states[TOURNER_DROITE          ] = &&STATE_TOURNER_DROITE;
        states[ROTATION45_GAUCHE       ] = &&STATE_ROTATION45_GAUCHE;
        states[ROTATION45_DROITE       ] = &&STATE_ROTATION45_DROITE;
        states[CHANGER_MUR_GAUCHE      ] = &&STATE_CHANGER_MUR_GAUCHE;
        states[CHANGER_MUR_DROITE      ] = &&STATE_CHANGER_MUR_DROITE;
        states[ROTATION45_UNDO_GAUCHE  ] = &&STATE_ROTATION45_UNDO_GAUCHE;
        states[ROTATION45_UNDO_DROITE  ] = &&STATE_ROTATION45_UNDO_DROITE;
    }

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

    void* state = states[robot->state];
    if(state != NULL) goto *state;
    
/*
    uart_printf("%d %d\n\r",
        capteurs->gauche.value,
        capteurs->droit.value
    );
*/
    return;

STATE_RESET:
    /* si on ne détecte rien, on reste à cet état */
    if((!capteurs->gauche.capting) && (!capteurs->droit.capting)) {
        robot->state = RESET;
        return;
    }

    /* on ne détecte rien à gauche, on va à droite */ 
    if(!capteurs->gauche.capting) {
        robot->state = AVANCER_DROITE;
        return;
    }

    /* on ne détecte rien à droite, on va à gauche */ 
    if(!capteurs->droit.capting) {
        robot->state = AVANCER_GAUCHE;
        return;
    }

    /* si on détecte de chaque coté, on va vers le plus près */
    if(capteurs->gauche.value < capteurs->droit.value) {
        robot->state = AVANCER_GAUCHE;
    } else {
        robot->state = AVANCER_DROITE;
    }
    return;

STATE_AVANCER_GAUCHE:
    /* on suit le mur gauche */
    robot->mur = GAUCHE;

    /* on s'ajuste par rapport au mur gauche */
    robot_ajustement(robot);

    /* on regarde si on capte quelque chose à droite */
    if(capteurs->droit.capting) {
        robot->state = VERIFIER_DROITE;
        return;
    }

    /* on regarde si on capte encore vers la gauche */
    if(!capteurs->gauche.capting) {
        robot->state = TOURNER_GAUCHE;
        return;
    }
    return;

STATE_AVANCER_DROITE:
    /* on suit le mur droite */
    robot->mur = DROITE;

    /* on s'ajuste par rapport au mur droite */
    robot_ajustement(robot);

    /* on regarde si on capte quelque chose à gauche */
    if(capteurs->gauche.capting) {
        robot->state = VERIFIER_GAUCHE;
        return;
    }

    /* on regarde si on capte encore vers la droite */
    if(!capteurs->droit.capting) {
        robot->state = TOURNER_DROITE;
        return;
    }
    return;

STATE_AVANCER_GAUCHE_ATTENDRE:
    /* on suit le mur gauche */
    robot->mur = GAUCHE;

    /* on s'ajuste par rapport au mur gauche */
    robot_ajustement(robot);

    /* le robot peut changer de bord si on ne capte rien à droite */
    if(!capteurs->droit.capting) {
        robot->state = AVANCER_GAUCHE;
        return;
    }

    /* si on ne capte plus rien à gauche, on tourne */
    if(!capteurs->gauche.capting) {
        robot->state = TOURNER_GAUCHE;
        moteur_tourner(GAUCHE);
        return;
    }
    return;

STATE_AVANCER_DROITE_ATTENDRE:
    /* on suit le mur droite */
    robot->mur = DROITE;

    /* on s'ajuste par rapport au mur droite */
    robot_ajustement(robot);

    /* le robot peut changer de bord si on ne capte rien à gauche */
    if(!capteurs->gauche.capting) {
        robot->state = AVANCER_DROITE;
        return;
    }

    /* si on ne capte plus rien à droite, on tourne */
    if(!capteurs->droit.capting) {
        robot->state = TOURNER_DROITE;
        moteur_tourner(DROITE);
        return;
    }
    return;

STATE_VERIFIER_GAUCHE:
    /* on suit le mur droite */
    robot->mur = DROITE;

    /* on s'ajuste par rapport au mur gauche */
    robot_ajustement(robot);

    /* on ignore les poteaux */
    if(obstacle == POTEAU) {
        robot->state = AVANCER_DROITE;
        return;
    }

    /* on change de mur si possible */
    if(obstacle == MUR) {
        uint16_t distance = capteurs->gauche.value;
        if(distance > 250) {
            TRANSITION_ROTATION45(robot, GAUCHE, distance);
        } else {
            robot->state = AVANCER_GAUCHE_ATTENDRE;
        }
        return;
    }
    return;

STATE_VERIFIER_DROITE:
    /* on suit le mur gauche */
    robot->mur = GAUCHE;

    /* on s'ajuste par rapport au mur droite */
    robot_ajustement(robot);

    /* on ignore les poteaux */
    if(obstacle == POTEAU) {
        robot->state = AVANCER_GAUCHE;
        return;
    }

    /* on change de mur si possible */
    if(obstacle == MUR) {
        uint16_t distance = capteurs->droit.value;
        if(distance > 250) {
            TRANSITION_ROTATION45(robot, DROITE, distance);
        } else {
            robot->state = AVANCER_DROITE_ATTENDRE;
        }
        return;
    }
    return;

STATE_VERIFIER_GAUCHE_ROTATION:
    /* on s'assure que le moteur est en rotation vers la droite */
    moteur_tourner(DROITE);

    /* à un poteau, on continue de tourner */
    if(obstacle == POTEAU) {
        robot->state = TOURNER_DROITE;
        return;
    }

    /* à un mur, on change de coté */
    if(obstacle == MUR) {
        robot->state = AVANCER_GAUCHE_ATTENDRE;
        return;
    }
    return;

STATE_VERIFIER_DROITE_ROTATION:
    /* on s'assure que le moteur est en rotation vers la gauche */
    moteur_tourner(GAUCHE);

    /* à un poteau, on continue de tourner */
    if(obstacle == POTEAU) {
        robot->state = TOURNER_GAUCHE;
        return;
    }

    /* à un mur, on change de coté */
    if(obstacle == MUR) {
        robot->state = AVANCER_DROITE_ATTENDRE;
        return;
    }
    return;

STATE_TOURNER_GAUCHE:
    /* on s'assure que le moteur est en rotation vers la gauche */
    moteur_tourner(GAUCHE);

    /* on regarde si on capte quelque chose vers la gauche */
    if(capteurs->gauche.capting) {
        robot->state = AVANCER_GAUCHE;
        return;
    }

    /* on vérifie ce qu'il y a à droite */
    if(capteurs->droit.capting) {
        robot->state = VERIFIER_DROITE_ROTATION;
        return;
    }
    return;

STATE_TOURNER_DROITE:
    /* on s'assure que le moteur est en rotation vers la droite */
    moteur_tourner(DROITE);

    /* on regarde si on capte quelque chose vers la droite */
    if(capteurs->droit.capting) {
        robot->state = AVANCER_DROITE;
        return;
    }

    /* on vérifie ce qu'il y a à droite */
    if(capteurs->gauche.capting) {
        robot->state = VERIFIER_GAUCHE_ROTATION;
        return;
    }
    return;

STATE_ROTATION45_GAUCHE:
    /* on actualise le timeout */
    robot->timeout -= DELAY;

    /* on change de mur */
    if(robot->timeout < 0) {
        TRANSITION_CHANGER_MUR(robot, GAUCHE);
        return;
    }

    /* on active les roues en rotation */
    moteur_tourner_surplace(GAUCHE);
    return;

STATE_ROTATION45_DROITE:
    /* on actualise le timeout */
    robot->timeout -= DELAY;

    /* on change de mur */
    if(robot->timeout < 0) {
        TRANSITION_CHANGER_MUR(robot, DROITE);
        return;
    }

    /* on active les roues en rotation */
    moteur_tourner_surplace(DROITE);
    return;

STATE_CHANGER_MUR_GAUCHE:
    /* on actualise le timeout */
    robot->timeout -= DELAY;

    /* on va vers le mur gauche */
    robot->mur = GAUCHE;

    /* on arrête d'avancer lorsque le timeout est finit */
    if((robot->timeout < 0) || (capteurs->gauche.value < 250)) {
        TRANSITION_ROTATION45_UNDO(robot, GAUCHE);
        return;
    }

    /* on avance */
    moteur_avancer(128);
    return;

STATE_CHANGER_MUR_DROITE:
    /* on actualise le timeout */
    robot->timeout -= DELAY;

    /* on va vers le mur droite */
    robot->mur = DROITE;

    /* on arrête d'avancer lorsque le timeout est finit */
    if((robot->timeout < 0) || (capteurs->droit.value < 250)) {
        TRANSITION_ROTATION45_UNDO(robot, DROITE);
        return;
    }

    /* on avance */
    moteur_avancer(128);
    return;

STATE_ROTATION45_UNDO_GAUCHE:
    /* on actualise le timeout */
    robot->timeout -= DELAY;

    /* on change de mur */
    if(robot->timeout < 0) {
        robot->state = AVANCER_GAUCHE_ATTENDRE;
        return;
    }

    /* on active les roues en rotation */
    moteur_tourner_surplace(DROITE);
    return;

STATE_ROTATION45_UNDO_DROITE:
    /* on actualise le timeout */
    robot->timeout -= DELAY;

    /* on change de mur */
    if(robot->timeout < 0) {
        robot->state = AVANCER_DROITE_ATTENDRE;
        return;
    }

    /* on active les roues en rotation */
    moteur_tourner_surplace(GAUCHE);
    return;
}
