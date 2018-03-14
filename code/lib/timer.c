#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <timer.h>

/* valeur par défaut du prescaler */
#ifndef TIMER_PRESCALER
    #define TIMER_PRESCALER 1024
#endif

/** Le temps maximal d'un timer 16-bits en millisecondes. */
#define MAX_TIME_MILLISECONDS 1000*((0xFFFF * (uint32_t) TIMER_PRESCALER)/F_CPU)

/** Cette structure peut être utilisée pour ne pas affecter un canal. */
struct callback CALLBACK_IGNORE = {
    .time = 0,
    .func = NULL,
    .repeat = 0,
};

/** Le callback pour le canal A du timer. */
struct callback* callbackA;

/** Le callback pour le canal B du timer. */
struct callback* callbackB;

/**
 * Cette fonction configure une interruption pour le canal A. Si la structure
 * pour le callback est `NULL`, alors l'interruption est totalement désactiver.
 * Si le temps du callback est de zéro, alors on ne touche pas au callback
 * actuel. Sinon, on remplace le callback actuel du canal B par celui dans la
 * structure.
 *
 * @param c  Un pointer vers une structure de callback.
 */
void configure_interruptA(struct callback* c);

/**
 * Cette fonction configure une interruption pour le canal B. Si la structure
 * pour le callback est `NULL`, alors l'interruption est totalement désactiver.
 * Si le temps du callback est de zéro, alors on ne touche pas au callback
 * actuel. Sinon, on remplace le callback actuel du canal B par celui dans la
 * structure.
 *
 * @param c Un pointer vers une structure de callback.
 */
void configure_interruptB(struct callback* c);

void timer_init(void) {
    /* on veut que le compteur soit déconnecté de sa pin */
    TCCR1A = 0;

    /* timer normale avec 0xFFFF comme top */
    TCCR1B = 0;

    /* on configure le prescaler */
#if   TIMER_PRESCALER == 1
    TCCR1B |= _BV(CS10);
#elif TIMER_PRESCALER == 8
    TCCR1B |= _BV(CS11);
#elif TIMER_PRESCALER == 64
    TCCR1B |= _BV(CS11) | _BV(CS10);
#elif TIMER_PRESCALER == 256
    TCCR1B |= _BV(CS12);
#elif TIMER_PRESCALER == 1024
    TCCR1B |= _BV(CS12) | _BV(CS10);
#else
    #error "Invalid timer prescaler value"
#endif

    /* ce registre n'est pas utilisé */
    TCCR1C = 0;
}

int8_t timer_start(struct callback* cA, struct callback* cB) {
    /* on s'assure que le temps d'attente ne dépasse pas la limite du timer */
    if(MAX_TIME_MILLISECONDS < cA->time) return INVALID_TIMEA;
    if(MAX_TIME_MILLISECONDS < cB->time) return INVALID_TIMEB;

    /* on désactive les interruptions */
    cli();

    /* on configure les interruptions */
    configure_interruptA(cA);
    configure_interruptB(cB);

    /* on active les interruptions */
    sei();

    return 0;
}

/** Cette macro gère une interruption. */
#define HANDLE_INTERRUPT(channel) {                           \
    /* on désactive les interruptions */                      \
    cli();                                                    \
                                                              \
    /* on appel le callback */                                \
    (*callback##channel->func)();                             \
                                                              \
    if(callback##channel->repeat)                             \
        /* on calcul le prochain compteur si on répète */     \
        OCR1##channel = TCNT1 + callback##channel->time;      \
    else                                                      \
        /* on désactive l'interruption si on ne répète pas */ \
        TIMSK1 &= ~_BV(OCIE1##channel);                       \
                                                              \
    /* on reactive les interruptions */                       \
    sei();                                                    \
}

/** Le vecteur d'interruption pour le canal A. */
ISR(TIMER1_COMPA_vect) {
    HANDLE_INTERRUPT(A);
}

/** Le vecteur d'interruption pour le canal B. */
ISR(TIMER1_COMPB_vect) {
    HANDLE_INTERRUPT(B);
}

/** Cette macro génère les fonctions pour configurer les interruptions. */
#define CONF_INTERRUPT(channel)                                       \
void configure_interrupt##channel(struct callback* ncallback) {       \
    /* on enlève le callback si aucune est spécifié */                \
    if(ncallback == NULL) {                                           \
        TIMSK1 &= ~_BV(OCIE1##channel);                               \
        callback##channel = NULL;                                     \
        return;                                                       \
    }                                                                 \
                                                                      \
    /* on ne touche pas au callback */                                \
    if(ncallback->time == 0) return;                                  \
                                                                      \
    /* on calcule le compteur nécessaire */                           \
    ncallback->time = (ncallback->time*(F_CPU/1000))/TIMER_PRESCALER; \
                                                                      \
    /* on change le callback */                                       \
    callback##channel = ncallback;                                    \
    TIMSK1 |= _BV(OCIE1##channel);                                    \
    OCR1##channel = TCNT1 + ncallback->time;                          \
}

/* on génère les fonctions */
CONF_INTERRUPT(A)
CONF_INTERRUPT(B)
