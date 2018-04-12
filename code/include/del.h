#ifndef INCLUDE_DEL_H
#define INCLUDE_DEL_H
#include <sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette fonction initialise la diode électro-luminescente. Donc, le port A
 * sera utilisé pour le contrôle des dels et sera en mode entrée.
 */
void del_init(void);

/**
 * Cette fonction permet d'allumer la DEL de la couleur rouge.
 */
void del_red(void);

/**
 * Cette fonction permet d'allumer la DEL de la couleur verte.
 */
void del_green(void);

/**
 * Cette fonction permet d'allumer la DEL de la couleur ambrée.
 */
void del_amber(void);

/**
 * Cette fonction permet d'éteindre la DEL.
 *
 * @param numero Le numéro de la PINA.
 */
void del_off(uint8_t numero);

/**
 * Cette fonction permet de choisir le numéro de la PINA désirée en mode sortie
 *
 * @param numero Le numéro de la PINA.
 */
void del_on(uint8_t numero);

/**
 * Cette fonction affiche si le robot est à la distance ou entrain de s'ajuster.
 *
 * @param la direction à suivre.
 */
void del_ajust(struct capteurs* capteurs, uint8_t direction);

/**
 * Cette fonction exécute une routine préprogrammée sur pour les DELs.
 */
void del_depart(void);

#ifdef __cplusplus
}
#endif

#endif
