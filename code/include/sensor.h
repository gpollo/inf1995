#ifndef INCLUDE_SENSOR_H
#define INCLUDE_SENSOR_H

#include <utils.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette structue définie un capteur de distance.
 */
struct capteur {
    /** Le numéro de la pin de l'ADC. */
    uint8_t pin;
    /** La dernière valeur brute lue. */
    int16_t raw;
    /** La dernière valeur calculée. */
    int16_t value;
    /** Si le capteur capte un objet ou non. */
    uint8_t capting;
    /** Ce compteur est utilisé pour déterminer si on capte ou non. */
    uint8_t counter;
};

/** Cette macro définit lorsque le capteur ne capte rien. */
#define NOT_CAPTING 0

/** Cette macro définit lorsque le capteur capte quelque chose. */
#define CAPTING 1

/**
 * Un initialisateur pour la structure d'un capteur.
 *
 * @param npin Le numéro de la pin ADC du capteur.
 */
#define CAPTEUR_INIT(npin) { \
    .pin     = (npin),       \
    .raw     = 0,            \
    .value   = 0,            \
    .capting = NOT_CAPTING,  \
    .counter = 0,            \
}

/**
 * Cette structure définie les deux capteurs de distance.
 */
struct capteurs {
	/** Le capteur de gauche. */
    struct capteur gauche;
    /** Le senseur de droite. */
    struct capteur droit;
};

/**
 * Un initialisateur pour la structure des capteurs.
 *
 * @param ping Le numéro de la pin ADC du capteur gauche.
 * @param pind Le numéro de la pin ADC du capteur droit.
 */
#define CAPTEURS_INIT(ping, pind) { \
    .gauche = CAPTEUR_INIT(ping),   \
    .droit  = CAPTEUR_INIT(pind),   \
}

/**
 * Cette fonction lit la valeur des capteurs de distance.
 * 
 * @param capteurs Un pointeur vers la structure des capteurs.
 */
void sensor_read(struct capteurs* capteurs);

/**
 * Cette fonction actualise l'état du capteur. C'est-à-dire que s'il ne captait
 * rien, mais qu'il a capté plusieurs valeurs valides de suite, alors il change
 * d'état. De même que s'il captait quelque chose, mais qu'il capte plusieurs
 * valeurs invalides de suite, alors il change d'état.
 *
 * Note: La macro #CAPTING_LIMIT control le nombre de valeurs de suite
 *       nécessaire afin de changer d'état.
 *
 * @param capteur Un pointer vers une structure d'un capteur.
 * @param capting Si le capteur a capté quelque chose.
 */
void sensor_update_capting(struct capteur* capteur, uint8_t capting);

/**
 * Cette fonction convertit une valeur analogique du capteur en distance.
 * 
 * @param capteur Un pointeur vers une structure d'un capteur.
 * 
 * @return Si la conversion fut effectuée, alors #OK, sinon #FAIL.
 */
uint8_t sensor_get_value(struct capteur* capteur);

/**
 * Cette fonction compare la distance cpaté à la distance de 15 cm souhaité.
 * 
 * @param capteur Un pointeur vers une structure d'un capteur et direction.
 * 
 * @return La différence entre la distance capté et la distance souhaité.
 */
int16_t sensor_diff_dist(struct capteurs* capteurs, enum direction direction);

#ifdef __cplusplus
}
#endif

#endif
