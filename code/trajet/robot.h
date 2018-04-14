#ifndef INCLUDE_ROBOT_H
#define INCLUDE_ROBOT_H

/**
 * Cette énumération définie les états possibles du robot.
 */
enum state {
    RESET,                    /*  0 */
    AVANCER_GAUCHE,           /*  1 */
    AVANCER_DROITE,           /*  2 */
    AVANCER_GAUCHE_ATTENDRE,  /*  3 */
    AVANCER_DROITE_ATTENDRE,  /*  4 */
    VERIFIER_GAUCHE,          /*  5 */
    VERIFIER_DROITE,          /*  6 */
    VERIFIER_GAUCHE_ROTATION, /*  7 */
    VERIFIER_DROITE_ROTATION, /*  8 */
    TOURNER_GAUCHE,           /*  9 */
    TOURNER_DROITE,           /* 10 */
    CHANGER_MUR_GAUCHE,       /* 11 */
    CHANGER_MUR_DROITE,       /* 12 */
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
};

/**
 * Cette énumération définie un obstacle.
 */
enum obstacle {
    UNKNOWN,
    MUR,
    POTEAU,
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
}

#endif


