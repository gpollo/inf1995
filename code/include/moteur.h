#ifndef INCLUDE_MOTEUR_H
#define INCLUDE_MOTEUR_H

#ifdef _cplusplus
extern "C" {
#endif

/**
 * Cette méthode initialise les moteurs du robot. À l'interne, elle l'utilise
 * les deux canaux du compteur TC0, c'est-à-dire un canal pour chaque roue.
 *
 * Note: La macro #MOTEUR_PRESCALER peut être utilisée pour controller le
 *       prescaler du compteur.
 */
void motor_init();

#ifdef _cplusplus
}
#endif

#endif
