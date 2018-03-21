#include <uart.h>
#include <mpu6050.h>
#include <util/delay.h>
#include <avr/io.h>

int main(void) {
    uart_init();
    mpu6050_init();

    uart_printf("\n\rbegin\n\r");

    mpu6050_set_sleep(0, 0);

 //   mpu6050_calibrate_gyroscope(0);
    mpu6050_set_gyroscope_offset_default(0);

    struct gyroscope gyro;
    while(1) {
        mpu6050_read_gyroscope(0, &gyro);
        uart_printf("\rgyro: x=%-6d, y=%-6d, z=%-6d", gyro.x, gyro.y, gyro.z);
    }
}

//1100
//-16500
//1000
