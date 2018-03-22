
#include <util/delay.h>
#include <memory.h>
#include <uart.h>

int main(void) {
    uart_init();
    memory_init();
    uint16_t addr = 0;
    uart_printf("\n\r");
    while (1) {
        uint8_t data = memory_read_byte(0, addr++);
        uart_printf("0x%02x\t", data);
        _delay_ms(500);
    }
}
