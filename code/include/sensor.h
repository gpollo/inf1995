#ifndef INCLUDE_SENSOR_H
#define INCLUDE_SENSOR_H

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
};

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
 * Cette fonction lit la valeur des capteurs de distance.
 * 
 * @param capteurs Un pointeur vers la structure des capteurs.
 */
void sensor_read(struct capteurs* capteurs);

/**
 * Cette fonction convertit une valeur analogique du capteur en distance.
 * 
 * @param capteur Un pointeur vers une structure d'un capteur.
 * 
 * @return Si la conversion fut effectuée, alors #OK, sinon #FAIL.
 */
uint8_t sensor_get_value(struct capteur* capteur);

/**
 * Cette fonction indique s'il y a un nouveau mur de l'autre côté
 * 
 * @return 0 pour un poteau détecté, 1 pour un mur détecté.
 */
uint8_t sensor_is_nouveau_mur (struct capteurs* capteurs, uint8_t direction);

#ifdef __cplusplus
}
#endif

#endif
