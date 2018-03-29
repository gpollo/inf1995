#include <avr/io.h>
#include <util/delay.h>
#include <uart.h>
#include <sensor.h>



int main(void) {
	DDRA = 0x00;
    uart_init();
    adc_init();
    uart_printf("\n\r");
   
    struct capteur capteurs;

    while (1) {
		sensor_read(&capteurs);
        int16_t data = distance_value(capteurs.gauche);
        uart_printf("%i\t", data);
        _delay_ms(500);

    }
}
