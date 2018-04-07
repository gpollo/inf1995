#ifndef INCLUDE_MOTEUR_H
#define INCLUDE_MOTEUR_H

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

#ifndef ROTATION_SPEED
            #define ROTATION_SPEED 200
#endif

#ifndef SPEED_STATIONARY
            #define SPEED_STATIONARY 0
#endif

#ifndef SPEED_ROTATION
            #define SPEED_ROTATION(vitesse) (((vitesse*255)/100))
#endif

/**
 * Cette méthode initialise les moteurs du robot. À l'interne, elle l'utilise
 * les deux canaux du compteur TC0, c'est-à-dire un canal pour chaque roue.
 *
 * Note: La macro #MOTEUR_PRESCALER peut être utilisée pour controller le
 *       prescaler du compteur.
 */
void moteur_init();

void moteur_avancer(uint8_t speed_ratio);

void moteur_reculer(uint8_t speed_ratio);

void moteur_arreter();

void moteur_tourner_droite(int8_t vitesse);

void moteur_tourner_gauche(int8_t vitesse);

#ifdef __cplusplus
}
#endif

#endif
