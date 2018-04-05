#include <avr/io.h>
#include <util/delay.h>
#include <uart.h>
#include <sensor.h>
#include <moteur.h>
#include <adc.h>

int main(void) {
	DDRA = 0x00;
    uart_init();
    moteur_init();
    adc_init();
    uart_printf("\n\r");
   
    struct capteurs capteurs;

    while (1) {
		sensor_read(&capteurs);
        moteur_ajustement(&capteurs, 0);
        int16_t data = sensor_get_distance(capteurs.gauche);
        _delay_ms(10);
    }
}
