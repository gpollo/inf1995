#include <avr/io.h>
#include <adc.h>

/* valeur par défaut du facteur de division */
#ifndef ADC_DIVISION_FACTOR
    #define ADC_DIVISION_FACTOR 64
#endif

/* on ne lit que les pins allant de 0 à 7 */
#define ADC_MAX_PIN 0x07

void adc_init(void) {
    /* aucune référence interne, aucun ajustement */
    ADMUX = 0;

    /* on active l'ADC */
    ADCSRA = _BV(ADEN);

    /* on configure le facteur de division */
#if   ADC_DIVISION_FACTOR == 2

#elif ADC_DIVISION_FACTOR == 4 
    ADCSRA |= _BV(ADPS1);
#elif ADC_DIVISION_FACTOR == 8
    ADCSRA |= _BV(ADPS1) | _BV(ADPS0);
#elif ADC_DIVISION_FACTOR == 16
    ADCSRA |= _BV(ADPS2);
#elif ADC_DIVISION_FACTOR == 32
    ADCSRA |= _BV(ADPS2) | _BV(ADPS0);
#elif ADC_DIVISION_FACTOR == 64
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1);
#elif ADC_DIVISION_FACTOR == 128
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
#else
    #error "Facteur de division invalide pour l'ADC"
#endif 
}

void adc_terminate(void) {
    /* on désactive l'ADC */
    ADCSRA &= ~_BV(ADEN);
}

uint16_t adc_read(uint8_t pos) {
    /* on efface les bits permettant la sélection de l'entrée */
    ADMUX &= ~(_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));

    /*
     * Note: Les bits 4 et 3 du multiplexeur ne permettent pas d'aller lire
     *       plus de 8 pins, mais bien de lire les 8 pins d'une manière
     *       différente qui n'est pas intêret dans notre cas.
     */

    /* on configure le multiplexeur pour aller lire la bonne entrée */
    ADMUX |= (pos & ADC_MAX_PIN) << MUX0;

    /* on démarre la conversion */
    ADCSRA |= _BV(ADSC);

    /* on attend la fin de la conversion (13 cycles dy convertisseur) */
    while(!(ADCSRA & _BV(ADIF)));

    /* on réactive le bit d'indication pour la prochaine conversion */
    ADCSRA |= _BV(ADIF);

    /* on retourne la valeur calculée */
    return ADCL | (ADCH << 8);
}
