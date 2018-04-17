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
#include "state.h"
#include "trajet.h"

void interrupteur(uint8_t button, void* data) {
    /* on obtient le robot en paramètre */
    struct robot* robot = (struct robot*) data;

    /* on s'assure que le bouton est appuyé */
    if(button) return;

    /* on change de mur pour effectuer une bonne rotation */
    robot->mur = (robot->mur == DROITE) ? GAUCHE : DROITE;

    /* on effectue une rotation */
    moteur_tourner180(robot->mur);

    /* on réinitialise des valeurs */
    robot->state = RESET;
    robot->mur = AUCUNE;
    robot->timeout = 0;
    robot->time = 0;
    robot->capteurs.gauche.value = 0;
    robot->capteurs.droit.value = 0;
}

void trajet_main(void) {
	struct robot robot = ROBOT_INIT(4, 3);
	
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

    /* on démarre le timer du changement d'états */
    state_start_timer(&robot);

    while(1);
}
