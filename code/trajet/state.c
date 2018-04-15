#include <stdlib.h>
#include <stdint.h>
#include <timer.h>
#include <utils.h>

#include "robot.h"
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

void update_state(void* data) {
    /* on obtient le robot */
    struct robot* robot = (struct robot*) data;
    UNUSED(robot);
}

void state_start_timer(struct robot* robot) {
    /* on envoie le robot au callback */
    state_timer.data = (void*) robot;

    /* on active le callback */
    timer_start(&CALLBACK_IGNORE, &state_timer);
}
