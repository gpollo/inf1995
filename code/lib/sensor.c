#include <avr/io.h>
#include <sensor.h>
#include <util/delay.h>

int8_t distance_value(int16_t Analog_Vo) {
    if(Analog_Vo > 1024)
        return;

    /* Cherche directement dans le tableau la valeur de distance selon le voltage */
    int8_t distance = sensor_tableau[Analog_Vo];
    return distance;
}

void sensor_read(struct capteur* capteur) {

    /* Lecture de l'état des capteurs */
    uint16_t capteur.gauche = adc_read(0);
    uint16_t capteur.droite = adc_read(1);
}
