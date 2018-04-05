#include <avr/io.h>
#include <adc.h>
#include <sensor.h>
#include <sensor_data.h>
#include <util/delay.h>
#include <timer.h>
#include <son.h>

#define POTEAU 1000

/** La table des distances. */
int16_t sensor_tableau[] = SENSOR_DATA;

void sensor_read(struct capteurs* capteurs) {
    /* on lit les deux capteurs */
    capteurs->gauche = adc_read(0);
    capteurs->droite = adc_read(1);
}

int16_t sensor_get_distance(int16_t valeur) {
	/* on s'assure que les valeurs lues sont dans la table */
    if(valeur < SENSOR_MIN || valeur > SENSOR_MAX) return 0;

	/* on calcule l'offset dans la table */
	valeur -= SENSOR_MIN;
    
    /* on obtient la distance à partir de la table */
    return sensor_tableau[valeur];
}

void son_poteau(void) {   
	son_jouer(77); 
	_delay_ms(200); 
	son_arreter();
	_delay_ms(100);
}

uint8_t sensor_is_nouveau_mur (struct capteurs* capteurs, uint8_t direction) {
	if (direction == 0) {
		while (sensor_get_distance(capteurs->droite) != 0) {
			_delay_ms(POTEAU);
			if (sensor_get_distance(capteurs->droite) != 0)
				return 1;
			else {
				son_poteau();
				son_poteau();
				son_poteau();
				return 0;
			}
		}
	} else {
		while (sensor_get_distance(capteurs->gauche) != 0) {
			_delay_ms(POTEAU);
			if (sensor_get_distance(capteurs->gauche) != 0)
				return 1;
			else {
				son_poteau();
				son_poteau();
				son_poteau();
				return 0;
			}
		}
	}
}
