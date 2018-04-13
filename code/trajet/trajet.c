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

#define DELAY 10

#define TIME_LIMIT 5000
#define TIME_MIN 300
#define TIME_MAX 600
#define TIME_MUR 1000

void buzzer_poteau(void* data);

void buzzer_caller(void);

enum state {
    RESET,                    /*  0 */
    AVANCER_GAUCHE,           /*  1 */
    AVANCER_DROITE,           /*  2 */
    AVANCER_GAUCHE_ATTENDRE,  /*  3 */
    AVANCER_DROITE_ATTENDRE,  /*  4 */
    VERIFIER_GAUCHE,          /*  5 */
    VERIFIER_DROITE,          /*  6 */
    VERIFIER_GAUCHE_ROTATION, /*  7 */
    VERIFIER_DROITE_ROTATION, /*  8 */
    TOURNER_GAUCHE,           /*  9 */
    TOURNER_DROITE,           /* 10 */
    CHANGER_MUR_GAUCHE,       /* 11 */
    CHANGER_MUR_DROITE,       /* 12 */
};

struct robot {
    struct capteurs capteurs;
    uint16_t time;
    uint8_t droite;
    enum direction mur;
    enum state state;
};

#define ROBOT_INIT(ping, pind) {           \
    .capteurs = CAPTEURS_INIT(ping, pind), \
    .time = 0,                             \
    .mur = AUCUNE,                         \
    .state = RESET,                        \
}

void trajet_bouton(uint8_t bouton, void* data) {
	if(bouton) return;
	
	//struct robot* robot = (struct robot*) data;
	//moteur_tourner180(robot->mur);
	// TODO state
	buzzer_caller();
}

enum obstacle {
    UNKNOWN,
    MUR,
    POTEAU,
};

struct callback buzzer_timer = { 
	.func = &buzzer_poteau,
	.time = 100,
	.repeat = 9,
	.data = NULL,
};

void buzzer_poteau(void* data) {
    UNUSED(data);
	switch(buzzer_timer.repeat % 3) {
	case 0: 
		buzzer_jouer();
		break;
	case 1: 
		buzzer_arreter();
		break;
	default:
        break;
	}
    buzzer_timer.repeat--;
}

void buzzer_caller(void) {
	buzzer_timer.repeat = 9;
    buzzer_timer.time = 100;
    timer_start(&buzzer_timer, &CALLBACK_IGNORE);
}

enum obstacle detect(struct robot* robot) {
    struct capteurs* capteurs = &(robot->capteurs);

    enum obstacle obstacle = UNKNOWN;

    if(robot->droite)
        if(capteurs->gauche.value != 0) {
            robot->time = LIMIT(robot->time+DELAY, TIME_LIMIT);
            if(robot->time > TIME_MUR) {
                obstacle = MUR;
            }
        } else {
            if((TIME_MIN < robot->time) && (robot->time < TIME_MAX)) {
                obstacle = POTEAU;
                buzzer_caller();
            }
            robot->time = 0;
        }
    else {
        if(capteurs->droit.value != 0) {
            robot->time = LIMIT(robot->time+DELAY, TIME_LIMIT);
            if(robot->time > TIME_MUR) {
                obstacle = MUR;
            }
        } else {
            if((TIME_MIN < robot->time) && (robot->time < TIME_MAX)) {
                obstacle = POTEAU;
                buzzer_caller();
            }
            robot->time = 0;
        }
    }

    return obstacle;
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

void update_state(struct robot* robot) {
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
        moteur_ajustement(capteurs, robot->droite);

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

    /* Del init */
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
	interruption_init(&trajet_bouton, (void*) &robot);

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
