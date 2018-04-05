#ifndef INCLUDE_MOTEUR_H
#define INCLUDE_MOTEUR_H

#include <sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DIR0A_DDR
            #define DIR0A_DDR DDRB
#endif

#ifndef DIR0A_PIN
            #define DIR0A_PIN PINB
#endif

#ifndef DIR0A_BIT
            #define DIR0A_BIT 2
#endif

#ifndef DIR0B_DDR
            #define DIR0B_DDR DDRB
#endif

#ifndef DIR0B_PIN
            #define DIR0B_PIN PINB
#endif

#ifndef DIR0B_BIT
            #define DIR0B_BIT 5
#endif

/* Define un pwm de 35% */
#ifndef ROTATION_SPEED
            #define ROTATION_SPEED 128
#endif

/**
 * Cette méthode initialise les moteurs du robot. À l'interne, elle l'utilise
 * les deux canaux du compteur TC0, c'est-à-dire un canal pour chaque roue.
 *
 * Note: La macro #MOTEUR_PRESCALER peut être utilisée pour controller le
 *       prescaler du compteur.
 */

/**
 * Cette méthode initialise les moteurs du robot. À l'interne, elle l'utilise
 * les deux canaux du compteur TC0, c'est-à-dire un canal pour chaque roue.
 *
 * Note: La macro #MOTEUR_PRESCALER peut être utilisée pour controller le
 *       prescaler du compteur.
 */
void moteur_init();

/**
 * Cette méthode permet au robot d'avance en ligne droite
 */
void moteur_avancer(uint8_t speed_ratio);

/**
 * Cette méthode permet au robot de reculer.
 */
void moteur_reculer(uint8_t speed_ratio);

/**
 * Cette méthode coupe les moteurs, ceux-ci ne peuvent fonctionnner à moins 
 * d'être réinitiliser.
 */
void moteur_arreter();

/**
 * Cette méthode effectue un virage de 90 degré vers la droite */
 */
void moteur_tourner_droite();

/**
 * Cette méthode effectue un virage de 90 degré vers la gauche */
 */
void moteur_tourner_gauche();

/**
 * Cette méthode s'assure de conserver une distance de 15 cm du mur
 */
void moteur_ajustement(struct capteurs* capteurs, uint8_t direction);

/**
 * Cette méthode effectue le déplacement pour commencer à suivre l'autre mur
 */
void changement_coter(struct capteurs* capteurs, uint8_t direction);

/**
 * Cette méthode effectue le déplacement pour commencer à suivre l'autre mur
 */
void temps_croissiere(uint16_t distance_a_faire);

#ifdef __cplusplus
}
#endif

#endif
