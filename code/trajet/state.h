#ifndef INCLUDE_STATE_H
#define INCLUDE_STATE_H

#include "robot.h"

/**
 * Cette fonction démarre le timer périodique pour l'actualisation de l'état.
 *
 * @param robot Un pointeur vers le robot.
 */
void state_start_timer(struct robot* robot);

#endif
