#ifndef INCLUDE_SENSOR_H
#define INCLUDE_SENSOR_H
#include "adc.h"

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
    uint16_t gauche;
    uint16_t droite;
};

int distance_value(uint16_t Analog_Vo);

void sensor_read(struct capteur* capteur);

#ifdef __cplusplus
}
#endif

#endif
