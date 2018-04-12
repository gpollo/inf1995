#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include <utils.h>
#include <sensor.h>
#include <moteur.h>
#include <adc.h>
#include <uart.h>
#include <timer.h>
#include <son.h>

#define DELAY 10

#define TIME_LIMIT 5000
#define TIME_MIN 300
#define TIME_MAX 600
#define TIME_MUR 1000

enum state {
    RESET,                   /*  0 */
    AVANCER_GAUCHE,          /*  1 */
    AVANCER_DROITE,          /*  2 */
    AVANCER_GAUCHE_ATTENDRE, /*  3 */
    AVANCER_DROITE_ATTENDRE, /*  4 */
    VERIFIER_GAUCHE,         /*  5 */
    VERIFIER_DROITE,         /*  6 */
    TOURNER_GAUCHE,          /*  7 */
    TOURNER_DROITE,          /*  8 */
    CHANGER_MUR_GAUCHE,      /*  9 */
    CHANGER_MUR_DROITE,      /* 10 */
};

struct robot {
    struct capteurs capteurs;
    uint16_t time;
    uint8_t droite;
    enum state state;
};

#define ROBOT_INIT(ping, pind) {           \
    .capteurs = CAPTEURS_INIT(ping, pind), \
    .time = 0,                             \
    .droite = FALSE,                       \
    .state = RESET,                        \
}

void son() {
    son_jouer(70);
    _delay_ms(100);
    son_arreter();
}

enum obstacle {
    UNKNOWN,
    MUR,
    POTEAU,
};

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
                son();
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
                son();
            }
            robot->time = 0;
        }
    }

    return obstacle;
}

enum state transition_reset(struct capteurs* capteurs) {

}

void update_state(struct robot* robot) {
    enum obstacle obstacle = detect(robot);

    struct capteurs* capteurs = &(robot->capteurs);

    uart_printf("%d ", robot->state);

    switch(robot->state) {
    case RESET:
        if((capteurs->gauche.value == 0) && (capteurs->droit.value == 0)) {
            robot->state = RESET;
            break;
        }

        if(capteurs->gauche.value == 0) {
            robot->state = AVANCER_DROITE;
            break;
        }

        if(capteurs->droit.value == 0) {
            robot->state = AVANCER_GAUCHE;
            break;
        }

        if(capteurs->gauche.value < capteurs->droit.value) {
            robot->state = AVANCER_GAUCHE;
        } else {
            robot->state = AVANCER_DROITE;
        }
        break;

    case AVANCER_GAUCHE:
        robot->droite = FALSE;
        moteur_ajustement(capteurs, robot->droite);
        if(capteurs->droit.value != 0) {
            robot->state = VERIFIER_DROITE;
            break;
        }

        if(capteurs->gauche.value == 0) {
            robot->state = TOURNER_GAUCHE;
            break;
        }
        break;

    case AVANCER_DROITE:
        robot->droite = TRUE;
        moteur_ajustement(capteurs, robot->droite);
        if(capteurs->gauche.value != 0) {
            robot->state = VERIFIER_GAUCHE;
            break;
        }

        if(capteurs->droit.value == 0) {
            robot->state = TOURNER_DROITE;
            break;
        }
        break;

    case AVANCER_GAUCHE_ATTENDRE:
        robot->droite = FALSE;
        moteur_ajustement(capteurs, robot->droite);
        if(capteurs->droit.value == 0) {
            robot->state = AVANCER_GAUCHE;
            break;
        }

        if(capteurs->gauche.value == 0) {
            robot->state = TOURNER_GAUCHE;
            break;
        }
        break;

    case AVANCER_DROITE_ATTENDRE:
        robot->droite = TRUE;
        moteur_ajustement(capteurs, robot->droite);
        if(capteurs->gauche.value == 0) {
            robot->state = AVANCER_DROITE;
            break;
        }

        if(capteurs->droit.value == 0) {
            robot->state = TOURNER_DROITE;
            break;
        }
        break;

    case VERIFIER_GAUCHE:
        robot->droite = TRUE;
        moteur_ajustement(capteurs, robot->droite);
        if(obstacle == POTEAU) {
            robot->state = AVANCER_DROITE;
        } else if(obstacle == MUR) {
            robot->state = CHANGER_MUR_GAUCHE;
        }
        break;

    case VERIFIER_DROITE:
        robot->droite = FALSE;
        moteur_ajustement(capteurs, robot->droite);
        if(obstacle == POTEAU) {
            robot->state = AVANCER_GAUCHE;
        } else if(obstacle == MUR) {
            robot->state = CHANGER_MUR_DROITE;
        }
        break;

    case TOURNER_GAUCHE:
        moteur_tourner180(0);
        robot->state = AVANCER_GAUCHE;
        break;

    case TOURNER_DROITE:
        moteur_tourner180(1);
        robot->state = AVANCER_DROITE;
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

int main(void) {
    uart_init();
    moteur_init();
    adc_init();
    son_init();
    uart_printf("\n\r");
 
    struct robot robot = ROBOT_INIT(0, 1);
    struct capteurs* capteurs = &(robot.capteurs);

    while(1) {
        sensor_read(capteurs);
        sensor_get_value(&(capteurs->gauche));
        sensor_get_value(&(capteurs->droit));
/*
        uart_printf("%3d %3d\n\r",
            capteurs->gauche.value,
            capteurs->droit.value
        );

        update_state(&robot);
*/

        uart_printf("%d %d -- %d %d\n\r",
            capteurs->droit.value / 10,
            capteurs->gauche.value / 10,
            capteurs->droit.capting,
            capteurs->gauche.capting
        );

        _delay_ms(DELAY);
    }
}
