#ifndef INCLUDE_ROBOT_H
#define INCLUDE_ROBOT_H

#include <sensor.h>

/* La distance a conserver */
#ifndef DISTANCE_SOUHAITEE
    #define DISTANCE_SOUHAITEE 150
#endif

/* La vitesse a 50 PWM */
#ifndef VITESSE_50PWM
    #define VITESSE_50PWM 10
#endif

/**
 * Cette énumération définie les états possibles du robot.
 */
enum state {
    RESET                    =  0,
    AVANCER_GAUCHE           =  1,
    AVANCER_DROITE           =  2,
    AVANCER_GAUCHE_ATTENDRE  =  3,
    AVANCER_DROITE_ATTENDRE  =  4,
    VERIFIER_GAUCHE          =  5,
    VERIFIER_DROITE          =  6,
    ROTATION45_GAUCHE        =  7,
    ROTATION45_DROITE        =  8,
    CHANGER_MUR_GAUCHE       =  9,
    CHANGER_MUR_DROITE       = 10,
    ROTATION45_UNDO_GAUCHE   = 11,
    ROTATION45_UNDO_DROITE   = 12,
    WAIT                     = 13,
};

/**
 * Cette structure définie le robot.
 */
struct robot {
    /** Les deux capteurs du robot. */
    struct capteurs capteurs;
    /** Un timer utiliser pour la détection de poteau. */
    uint16_t time;
    /** La direction à laquelle on suit un mur. */
    enum direction mur;
    /** L'état du robot. */
    enum state state;
    /** Le prochain état après l'état WAIT. */
    enum state next;
    /** Le timeout pour l'état WAIT. */
    int32_t wait;
    /** Un timeout pour certains états. */
    int32_t timeout;
    /** Une variable pour enregistrer une distance entre les états. */
    uint32_t distance;
};

/**
 * Cette macro initialise une structure pour le robot.
 *
 * @param ping La pin ADC du capteur gauche.
 * @param pind La pin ADC du capteur droit.
 */
#define ROBOT_INIT(ping, pind) {           \
    .capteurs = CAPTEURS_INIT(ping, pind), \
    .time = 0,                             \
    .mur = AUCUNE,                         \
    .state = RESET,                        \
    .timeout = 0,                          \
    .distance = 0,                         \
}

#define GET_TRAVEL_TIME(distance) ((100*((distance))))

/**
 * Cette fonction retourne l'erreur de la distance souhaitée du robot par
 * rapport au mur.
 *
 * @param robot Un pointeur vers le robot.
 *
 * @return L'erreur par rapport à la distance souhaitée.
 */
int16_t robot_get_erreur(struct robot* robot);

/**
 * Cette fonction ajuste les moteurs du robot pour conserver une distance au
 * mur.
 *
 * @param robot Un pointeur vers un robot.
 */
void robot_ajustement(struct robot* robot);

#endif


