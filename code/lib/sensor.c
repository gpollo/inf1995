#include <avr/io.h>
#include <adc.h>
#include <sensor.h>
#include <sensor_data.h>
#include <util/delay.h>

/** La table des distances. */
int16_t sensor_tableau[] = SENSOR_DATA;

void sensor_read(struct capteurs* capteurs) {
    /* on lit les deux capteurs */
    capteurs->gauche = adc_read(0);
    capteurs->droite = adc_read(1);
}

int16_t sensor_get_distance(int16_t valeur) {
	/* on s'assure que les valeurs lues sont dans la table */
    if(valeur < SENSOR_MIN || valeur > SENSOR_MAX) return -1;

	/* on calcule l'offset dans la table */
	valeur -= SENSOR_MIN;
    
    /* on obtient la distance à partir de la table */
    return sensor_tableau[valeur];
}
