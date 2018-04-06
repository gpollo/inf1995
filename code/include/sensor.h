#ifndef INCLUDE_SENSOR_H
#define INCLUDE_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette structue définie un capteur analogique.
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
 * Cette structure définie les deux senseurs de distance.
 */
struct capteurs {
	/** Le senseur de gauche. */
    int16_t gauche;
    /** Le senseur de droite. */
    int16_t droite;
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
 * @param valeur La valeur analogique du capteur.
 * 
 * @return La distance du capteur ou -1 si la valeur est invalide.
 */
int16_t sensor_get_distance(int16_t valeur);

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
