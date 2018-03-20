#ifndef INCLUDE_DEL_H
#define INCLUDE_DEL_H

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
void del_red();

/**
 * Cette fonction permet d'allumer la DEL de la couleur verte.
 */
void del_green();

/**
 * Cette fonction permet d'allumer la DEL de la couleur ambrée.
 */
void del_amber();

/**
 * Cette fonction permet d'éteindre la DEL.
 *
 * @param numero Le numéro de la PINA.
 */
void del_off();

/**
 * Cette fonction permet de choisir le numéro de la PINA désirée en mode sortie
 *
 * @param numero Le numéro de la PINA.
 */
void del_on(uint_8 numero);

#ifdef __cplusplus
}
#endif

#endif
