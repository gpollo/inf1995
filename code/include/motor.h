#ifndef INCLUDE_MOTOR_H
#define INCLUDE_MOTOR_H

/**
 * Cette méthode initialise les moteurs du robot. À l'interne, elle l'utilise
 * les deux canaux du compteur TC0, c'est-à-dire un canal pour chaque roue.
 *
 * La macro #PRESCALER peut être utilisée pour controller le prescaler du
 * compteur.
 */
void motor_init();

#endif
