#include <uart.h>
#include <memory.h>
#include <moteur.h>
#include <adc.h>
#include <util/delay.h>

void uart_test(void) {
    uart_init();

    /* on test l'écriture d'un string */
    uart_printf("\n\r");
    uart_printf("Hello World!\n\r");

    /* buffer pour l'entrée */
    uint8_t buffer[255];

    /* on demande une entrée par UART */
    uart_printf("Veuillez entrer une ligne: ");
    uart_getline(sizeof(buffer), buffer);
    uart_printf("Vous avez écrit: %s\n\r", buffer);
}

void memory_test(void) {
    memory_init();

    /* on test l'écriture/lecture d'octets */
    memory_write_byte(0, 0x0, 0xD);
    memory_write_byte(0, 0x1, 0xE);
    memory_write_byte(0, 0x2, 0xA);
    memory_write_byte(0, 0x3, 0xD);
    memory_write_byte(0, 0x4, 0xB);
    memory_write_byte(0, 0x5, 0xE);
    memory_write_byte(0, 0x6, 0xE);
    memory_write_byte(0, 0x7, 0xF);
    uart_printf("%x\n\r", memory_read_byte(0, 0x0));
    uart_printf("%x\n\r", memory_read_byte(0, 0x1));
    uart_printf("%x\n\r", memory_read_byte(0, 0x2));
    uart_printf("%x\n\r", memory_read_byte(0, 0x3));
    uart_printf("%x\n\r", memory_read_byte(0, 0x4));
    uart_printf("%x\n\r", memory_read_byte(0, 0x5));
    uart_printf("%x\n\r", memory_read_byte(0, 0x6));
    uart_printf("%x\n\r", memory_read_byte(0, 0x7));

    /* buffer utilisé pour lire les données */
    uint8_t buffer[255];

    /* on test l'écriture/lecture d'un bloc */
    char msg1[] = "Hello Memory!";
    memory_write_block(0, 0x8, sizeof(msg1), (uint8_t*) msg1);
    memory_read_block(0, 0x8, sizeof(msg1), buffer);
    uart_printf("%s\n\r", buffer);

    /* on test l'écriture/lecture d'un plus long bloc (>64) */
    char msg2[] = "The quick brown fox jumps over the lazy dog. " \
                  "My faxed joke won a pager in the cable TV quiz show.";
    memory_write_block(0, 0x8, sizeof(msg2), (uint8_t*) msg2);
    memory_read_block(0, 0x8, sizeof(msg2), buffer);
    uart_printf("%s\n\r", buffer);
}

void adc_test(void) {
    adc_init();
    
    uart_printf("Reading all ADC pins...\n\r");
    while(1) {
        /* on lit tout les pins */
        uart_printf("%4d ",   adc_read(0));
        uart_printf("%4d ",   adc_read(1));
        uart_printf("%4d ",   adc_read(2));
        uart_printf("%4d ",   adc_read(3));
        uart_printf("%4d ",   adc_read(4));
        uart_printf("%4d ",   adc_read(5));
        uart_printf("%4d ",   adc_read(6));
        uart_printf("%d\n\r", adc_read(7));

        /* on attend une demi seconde */
        _delay_ms(500);
    }
}

int main(void) {
    uart_test();
    memory_test();
    adc_test();
    return 0;
}
