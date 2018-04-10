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
#define TIME_MIN 500
#define TIME_MAX 700

enum state {
    RESET,
    AVANCER_GAUCHE,
    AVANCER_DROITE,
    AVANCER_GAUCHE_ATTENDRE,
    AVANCER_DROITE_ATTENDRE,
    VERIFIER_GAUCHE,
    VERIFIER_DROITE,
    TOURNER_GAUCHE,
    TOURNER_DROITE,
    CHANGER_MUR_GAUCHE,
    CHANGER_MUR_DROITE,
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
    MUR,
    POTEAU,
};

enum obstacle detect(struct robot* robot) {
    struct capteurs* capteurs = &(robot->capteurs);

    enum obstacle obstacle = MUR;

    if(robot->droite)
        if(capteurs->gauche.value != 0) {
            robot->time = LIMIT(robot->time+DELAY, TIME_LIMIT);
        } else {
            if((TIME_MIN < robot->time) && (robot->time < TIME_MAX)) {
                obstacle = POTEAU;
                son();
            } else {
                obstacle = MUR;
            }
            robot->time = 0;
        }
    else {
        if(capteurs->droit.value != 0) {
            robot->time = LIMIT(robot->time+DELAY, TIME_LIMIT);
        } else {
            if((TIME_MIN < robot->time) && (robot->time < TIME_MAX)) {
                obstacle = POTEAU;
                son();
            } else {
                obstacle = MUR;
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

    uart_printf("%d\n\r", robot->state);

    switch(robot->state) {
    case RESET:
        if((capteurs->gauche.value == 0) && (capteurs->droit.value == 0)) {
            robot->state = RESET;
            break;
        }

        if(capteurs->gauche.value == 0) {
            robot->state = AVANCER_DROITE;
            robot->droite = TRUE;
            break;
        }

        if(capteurs->droit.value == 0) {
            robot->state = AVANCER_GAUCHE;
            robot->droite = FALSE;
            break;
        }

        if(capteurs->gauche.value < capteurs->droit.value) {
            robot->state = AVANCER_GAUCHE;
            robot->droite = FALSE;
        } else {
            robot->state = AVANCER_DROITE;
            robot->droite = TRUE;
        }
        break;

    case AVANCER_GAUCHE:
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
        break;
    case AVANCER_DROITE_ATTENDRE:
        break;
    case VERIFIER_GAUCHE:
        moteur_ajustement(capteurs, robot->droite);
        if(obstacle == POTEAU) {

        } else {
            robot->state = AVANCER_DROITE;
        }
        break;
    case VERIFIER_DROITE:
        moteur_ajustement(capteurs, robot->droite);
        if(obstacle == POTEAU) {

        } else {
            robot->state = AVANCER_GAUCHE;
        }
        break;
    case TOURNER_GAUCHE:
        break;
    case TOURNER_DROITE:
        break;
    case CHANGER_MUR_GAUCHE:
        break;
    case CHANGER_MUR_DROITE:
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

//        moteur_ajustement(capteurs, 0);
        update_state(&robot);

        _delay_ms(DELAY);
    }
}
