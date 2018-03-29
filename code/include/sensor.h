#ifndef INCLUDE_SENSOR_H
#define INCLUDE_SENSOR_H
#include <adc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette méthode initialise les moteurs du robot. À l'interne, elle l'utilise
 * les deux canaux du compteur TC0, c'est-à-dire un canal pour chaque roue.
 *
 * Note: La macro #MOTEUR_PRESCALER peut être utilisée pour controller le
 *       prescaler du compteur.
 */
struct capteur {
    int16_t gauche;
    int16_t droite;
};

int16_t distance_value(int16_t capteurs);

void sensor_read(struct capteur* capteurs);

#ifdef __cplusplus
}
#endif

#endif
