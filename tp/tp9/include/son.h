#ifndef INCLUDE_SON_H
#define INCLUDE_SON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette méthode initialise le piezoelectrique.
 */
void son_init(void);

/**
 * Cette méthode fait jouer un son selon une table de fréquences MIDI.
 * 
 * @param numero Le numéro de la fréquence.
 */
void son_jouer(uint8_t numero);

/**
 * Cette méthode arrête le piezoelectrique.
 */
void son_arreter(void);

/**
 * Cette méthode émet une mélodie de démarrage.
 */
void son_depart(void) ;

#ifdef __cplusplus
}
#endif

#endif
