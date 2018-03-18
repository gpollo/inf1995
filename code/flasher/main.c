#include <uart.h>
#include <memory.h>

int main(void) {
    /* on initialise la communication UART */
    uart_init();

    /* on initialise la mémoire */
    memory_init();

    /* on commence à écrire à l'adresse zéro */
    uint16_t addr = 0;

    /* on écrit chaque octet lut à la mémoire */
    while(1) {
        /* on lit un octet */
        uint8_t octet = uart_receive();

        /* on l'écrit à la mémoire */
        memory_write_byte(0, addr++, octet);
    }
}
