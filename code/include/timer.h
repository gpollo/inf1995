#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette structure est utilisée pour définir une fonction qu'on désire appelée
 * après un certain temps.
 */
struct callback {
    /** Le temps d'attente en millisecondes. */
    uint16_t time;

    /** La fonction à appeler après que le temps soit écoulé. */
    void (*func)(void);
};

/** Cette structure peut être utilisée pour ne pas affecter un canal. */
extern const struct callback CALLBACK_IGNORE;

/** Code d'erreur pour un temps invalide du canal A. */
#define INVALID_TIMEA (-1)

/** Code d'erreur pour un temps invalide du canal B. */
#define INVALID_TIMEB (-2)

/**
 * Cette fonction initialise le timer 1.
 *
 * Note : La valeur du prescaler peut être controlée à l'aide de la macro
 *        `TIMER_PRESCALER`.
 */
void timer_init(void);

/**
 * Cette fonction configure les canaux du timer 1.
 *
 * @param cA Un pointeur vers une structure de callback pour le canal A.
 * @param cA Un pointeur vers une structure de callback pour le canal B.
 *
 * Note : Si le temps demandé est trop long pour le timer, une des valeurs
 *        `INVALID_TIMEA` ou `INVALID_TIMEB` est retournée.
 *
 * @return Si une erreur est survenue, une valeur négative est retournée.
 */
int8_t timer_start(struct callback* cA, struct callback* cB);

#ifdef __cplusplus
}
#endif

#endif
