#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <interrupt.h>
#include <utils.h>
#include <sensor.h>
#include <moteur.h>
#include <adc.h>
#include <uart.h>
#include <timer.h>
#include <del.h>
#include <buzzer.h>

#include "robot.h"
#include "buzz.h"
#include "obstacle.h"
#include "trajet.h"

void interrupteur(uint8_t button, void* data) {
    /* on obtient le robot en paramètre */
    struct robot* robot = (struct robot*) data;

    /* on s'assure que le bouton est appuyé */
    if(button) return;

    /* TODO: faire une rotation 180 et mettre l'état à RESET */
    UNUSED(robot);
}

#define CHECK_OBSTACLE(obs,robot,avancer,changer) { \
    switch((obs)) {                                 \
    case POTEAU:                                    \
        (robot)->state = AVANCER_##avancer;         \
        break;                                      \
    case MUR:                                       \
        (robot)->state= CHANGER_MUR_##changer;      \
        moteur_tourner((robot)->mur);               \
        break;                                      \
    default:                                        \
        break;                                      \
    }                                               \
}

void updatestate(struct robot* robot) {
    enum obstacle obstacle = detect(robot);

    struct capteurs* capteurs = &(robot->capteurs);

    uart_printf("%d ", robot->state);

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

        /* on change d'état selon l'obstacle */
        CHECK_OBSTACLE(obstacle, robot, DROITE, GAUCHE);
        break;

    case VERIFIER_DROITE:
        /* on suit le mur droite */
        robot->mur = DROITE;

        /* on s'ajuste par rapport au mur droite */
        moteur_ajustement(capteurs, robot->mur);

        /* on change d'état selon l'obstacle */
        CHECK_OBSTACLE(obstacle, robot, GAUCHE, DROITE);
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

void trajet_main(void) {
	struct robot robot = ROBOT_INIT(0, 1);
	
    /* pour le debugging */
    uart_init();

    /* pour la del */
    del_init();

    /* pour les moteurs */
    moteur_init();

    /* pour les capteurs */
    adc_init();

    /* pour la détection d'obstacle */
    buzzer_init();

    /* pour les timers */
    timer_init();

	/* pour le bouton interrupt */
	interruption_init(&interrupteur, (void*) &robot);

    /* on recommence une nouvelle ligne dans le deboggage */
    uart_printf("\n\r");

    /* le pointeur vers les capteurs */
    struct capteurs* capteurs = &(robot.capteurs);

    while(1) {
        /* on lit les capteurs */
        sensor_read(capteurs);
        sensor_get_value(&(capteurs->gauche));
        sensor_get_value(&(capteurs->droit));
        
        moteur_ajustement(capteurs, DROITE);
        del_ajust(capteurs, DROITE);
/*
        uart_printf("%d %d -- %d %d\n\r",
            capteurs->droit.value / 10,
            capteurs->gauche.value / 10,
            capteurs->droit.capting,
            capteurs->gauche.capting
        );
*/
        _delay_ms(DELAY);
    }
}
