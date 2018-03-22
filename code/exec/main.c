#include <uart.h>
#include <mpu6050.h>
#include <util/delay.h>
#include <avr/io.h>
#include <moteur.h>
#include <timer.h>

int main(void) {
    uart_init();
    mpu6050_init();
    moteur_init();
    timer_init();
    uart_printf("\n\rbegin\n\r");

    mpu6050_set_sleep(0, 0);


    mpu6050_set_gyroscope_offset_default(0);

 //   moteur_tourner_gauche();
    moteur_suivre(0);
    while(1);

}

