#include <avr/io.h>
#include <adc.h>
#include <sensor.h>
#include <sensor_data.h>
#include <util/delay.h>
#include <timer.h>
#include <utils.h>

/* combien de données il faut pour pouvoir capter */
#ifndef CAPTING_LIMIT
    #define CAPTING_LIMIT 10
#endif

/** La table des distances. */
int16_t sensor_tableau[] = SENSOR_DATA;

void sensor_read(struct capteurs* capteurs) {
    /* on lit les deux capteurs */
    capteurs->gauche.raw = adc_read(capteurs->gauche.pin);
    capteurs->droit.raw  = adc_read(capteurs->droit.pin);
}

void sensor_update_capting(struct capteur* capteur, uint8_t capting) {
    if(capteur->capting == CAPTING) {
        /* increment only when not capting, else reset count */
        if(capting == NOT_CAPTING) {
            capteur->counter++;
        } else {
            capteur->counter = 0;
        }

        /* if we didn't capted something for a while, then we're not capting */
        if(capteur->counter > CAPTING_LIMIT) {
            capteur->capting = NOT_CAPTING;
            capteur->counter = 0;
        }
    } else {
        /* increment only when capting, else reset count */
        if(capting == CAPTING) {
            capteur->counter++;
        } else {
            capteur->counter = 0;
        }

        /* if we're capting for a while, then we're capting */
        if(capteur->counter > CAPTING_LIMIT) {
            capteur->capting = CAPTING;
            capteur->counter = 0;
        }
    }
}

uint8_t sensor_get_value(struct capteur* capteur) {
	/* on s'assure que les valeurs lues sont dans la table */
    if(capteur->raw < SENSOR_MIN || capteur->raw > SENSOR_MAX) {
        sensor_update_capting(capteur, NOT_CAPTING);
        capteur->value = 0;
        return FAIL;
    } else {
        sensor_update_capting(capteur, CAPTING);
    }

	/* on calcule l'offset dans la table */
	uint16_t offset = capteur->raw - SENSOR_MIN;
    
    /* on obtient la distance à partir de la table */
    capteur->value = sensor_tableau[offset];

    /* la conversion s'est bien effectuée */
    return OK;
}
