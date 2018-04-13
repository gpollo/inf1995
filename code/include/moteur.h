#ifndef INCLUDE_MOTEUR_H
#define INCLUDE_MOTEUR_H

#include <sensor.h>
#include <utils.h>

#ifdef __cplusplus
extern "C" {
#endif

/* macros pour la pin de direction de la roue droite */
#ifndef DIR_DROITE_PIN
    #define DIR_DROITE_PIN PINB
#endif

#ifndef DIR_DROITE_BIT
    #define DIR_DROITE_BIT 2
#endif

#ifndef DIR_DROITE_DDR
    #define DIR_DROITE_DDR DDRB
#endif

#ifndef DIR_DROITE_PORT
    #define DIR_DROITE_PORT PORTB
#endif

/* macros pour la pin de direction de la roue gauche */
#ifndef DIR_GAUCHE_PIN
    #define DIR_GAUCHE_PIN PINB
#endif

#ifndef DIR_GAUCHE_BIT
    #define DIR_GAUCHE_BIT 5
#endif

#ifndef DIR_GAUCHE_DDR
    #define DIR_GAUCHE_DDR DDRB
#endif

#ifndef DIR_GAUCHE_PORT
    #define DIR_GAUCHE_PORT PORTB
#endif

/* macros pour la pin de controle de la vitesse de la roue droite */
#ifndef SPEED_DROITE_PIN
    #define SPEED_DROITE_PIN OC0A_PIN
#endif

#ifndef SPEED_DROITE_BIT
    #define SPEED_DROITE_BIT OC0A_BIT
#endif

#ifndef SPEED_DROITE_DDR
    #define SPEED_DROITE_DDR OC0A_DDR
#endif

#ifndef SPEED_DROITE_PORT
    #define SPEED_DROITE_PORT OC0A_PORT
#endif

#ifndef SPEED_DROITE
    #define SPEED_DROITE OCR0A
#endif

/* macros pour la pin de controle de la vitesse de la roue droite */
#ifndef SPEED_GAUCHE_PIN
    #define SPEED_GAUCHE_PIN OC0B_PIN
#endif

#ifndef SPEED_GAUCHE_BIT
    #define SPEED_GAUCHE_BIT OC0B_BIT
#endif

#ifndef SPEED_GAUCHE_DDR
    #define SPEED_GAUCHE_DDR OC0B_DDR
#endif

#ifndef SPEED_GAUCHE_PORT
    #define SPEED_GAUCHE_PORT OC0B_PORT
#endif

#ifndef SPEED_GAUCHE
    #define SPEED_GAUCHE OCR0B
#endif

/**
 * Cette macro active une roue en mode avancer.
 *
 * @param La roue DROITE ou GAUCHE.
 */
#define SET_DIRECTION_AVANCER(roue) {            \
    DIR_##roue##_PORT &=~ _BV(DIR_##roue##_BIT); \
}

/**
 * Cette macro active une roue en mode recule.
 *
 * @param La roue DROITE ou GAUCHE.
 */
#define SET_DIRECTION_RECULER(roue) {           \
    DIR_##roue##_PORT |= _BV(DIR_##roue##_BIT); \
}

/**
 * Cette macro change la vitesse de la roue.
 *
 * @param La roue DROITE ou GAUCHE.
 * @param La vitesse de la roue sur 8 bits.
 */
#define SET_SPEED(roue, speed) { \
    SPEED_##roue = (speed);      \
}

/**
 * Cette structure définie un moteur.
 */
struct moteur {
    /** Si le moteur avance ou pas. */
    uint8_t avancer;
    /** La vitesse du moteur. */
    uint8_t speed;
};

/**
 * Cette structure définie l'ensemble des moteurs.
 */
struct moteurs {
    /** Le moteur droit. */
    struct moteur droit;
    /** Le moteur gauche. */
    struct moteur gauche;
};

/**
 * Cette méthode initialise les moteurs du robot. À l'interne, elle l'utilise
 * les deux canaux du compteur TC0, c'est-à-dire un canal pour chaque roue.
 *
 * Note: La macro #MOTEUR_PRESCALER peut être utilisée pour controller le
 *       prescaler du compteur.
 */
void moteur_init();

/**
 * Cette méthode active les deux moteurs du robot vers l'avant.
 *
 * @param speed La vitesse du moteur.
 */
void moteur_avancer(uint8_t speed);

/**
 * Cette méthode active les deux moteurs du robot vers l'arrière.
 *
 * @param speed La vitesse du robot.
 */
void moteur_reculer(uint8_t speed);

/**
 * Cette méthode arrête les deux moteurs.
 */
void moteur_arreter();

/**
 * Cette méthode controle la roue de droite en pourcentage négatif ou positif.
 *
 * @param L'activation du moteur droit.
 */
void moteur_controler_droite(int8_t vitesse);

/**
 * Cette méthode controle la roue de gauche en pourcentage négatif ou positif.
 *
 * @param L'activation du moteur gauche.
 */
void moteur_controler_gauche(int8_t vitesse);

/**
 * Cette méthode effectue un virage de 90 degré vers la droite sur place.
 *
 * Note: La macro #DELAY_ROTATION90 peut être utilisée pour ajuster la rotation.
 */
void moteur_tourner_droite();

/**
 * Cette méthode effectue un virage de 90 degré vers la gauche sur place.
 *
 * Note: La macro #DELAY_ROTATION90 peut être utilisée pour ajuster la rotation.
 */
void moteur_tourner_gauche();

/**
 * Cette méthode active les roues en rotation vers une direction.
 *
 * @param direction La direction du robot.
 */
void moteur_tourner(enum direction);

/**
 * Cette méthode effectue un virage de 180 degré vers la direction souhaite.
 *
 * Note: La macro #DELAY_ROTATION90 peut être utilisée pour ajuster la rotation.
 */
void moteur_tourner180(enum direction);

/**
 * Cette méthode configure les deux moteurs.
 *
 * @param moteurs Un pointeur vers la configuration des deux moteurs.
 */
void moteur_config(struct moteurs* moteurs);

/**
 * Cette méthode s'assure de conserver une distance de 15 cm du mur
 */
void moteur_ajustement(struct capteurs* capteurs, enum direction direction);

/**
 * Cette méthode effectue le déplacement pour commencer à suivre l'autre mur
 */
void changement_coter(struct capteurs* capteurs, enum direction direction);

/**
 * Cette méthode effectue le déplacement pour commencer à suivre l'autre mur
 */
uint16_t temps_croissiere(uint16_t distance_a_faire);

#ifdef __cplusplus
}
#endif

#endif
