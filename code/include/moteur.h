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

/* Active les moteurs */
void moteur_init();

/* Avance en ligne droite */
void moteur_avancer(uint8_t speed_ratio);

/* Recule en ligne droite */
void moteur_reculer(uint8_t speed_ratio);

/* Arrete les moteurs */
void moteur_arreter();

/* Effectue un virage de 90 degre a droite */
void moteur_tourner_droite();

/* Effectue un virage de 90 degre a gauche */
void moteur_tourner_gauche();

/* Garde le robot a une distance de 15 cm de l'objet */
void moteur_ajustement(struct capteurs* capteurs, uint8_t direction);

/* Effectue le déplacement pour commencer à suivre l'autre mur */
void changement_coter(uint8_t direction);

#ifdef __cplusplus
}
#endif

#endif
