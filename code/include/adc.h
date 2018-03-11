#ifndef INCLUDE_CAN_H
#define INCLUDE_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette fonction initialise l'ADC avec aucune référence interne et aucune
 * ajustement de la tension.
 *
 * Note: La macro #ADC_DIVISION_FACTOR peut être utilisée pour controller le
 *       facteur de division de l'ADC.
 */
void adc_init(void);

/**
 * Cette fonction désactive l'ADC.
 */
void adc_terminate(void);

/**
 * Cette fonction lit la valeur d'une pin avec l'ADC.
 *
 * Note: Il y a 8 pins, allant de 0 à 7.
 *
 * @param pin La pin qu'on souhaite lire.
 *
 * @return La valeur de l'ADC en prenant compte du facteur de division.
 */
uint16_t adc_read(uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif
