#include <stdlib.h>
#include <buzzer.h>
#include <timer.h>
#include <utils.h>

#include "buzz.h"

/**
 * Cette fonction est appelée par un timer pour effectuer les 3 sons.
 *
 * @param data Un pointer vers des données reçus.
 */
void buzzer_callback(void* data);

/** Cette structure configure le callback pour le buzzer. */
struct callback buzzer_timer = {
    .func = &buzzer_callback,
    .time = 100,
    .repeat = 9,
    .data = NULL,
};

void buzzer_callback(void* data) {
    /* cette variable n'est pas utilisée */
    UNUSED(data);

    /* on active/désactive selon le callback */
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

    /* on décrémente la répétition */
    buzzer_timer.repeat--;
}

void buzz() {
    /* on réinitialise les répétitions */
    buzzer_timer.repeat = 9;

    /* on active le callback */
    timer_start(&buzzer_timer, &CALLBACK_IGNORE);
}
