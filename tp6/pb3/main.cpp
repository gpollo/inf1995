#define F_CPU 8000000
#define BAUD 2400

#include <avr/io.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "memoire_24.h"

#define UART_BUFFER_SIZE 255
#define FLASH_BUFFER_SIZE 16

/* the buffer shouldn't be bigger than 255 */
char buffer[UART_BUFFER_SIZE];

void uart_init(void)
{
    DDRD = 0xff;
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    /* enable RX and TX */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    /* 8 bits data */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void uart_putchar(char c) {
    /* wait until the data register is empty */
    loop_until_bit_is_set(UCSR0A, UDRE0);

    /* write the character */
    UDR0 = c;
}

int8_t uart_printf(char* format, ...)
{
    va_list args;
    va_start(args, format);

    int8_t len = vsnprintf(buffer, UART_BUFFER_SIZE, format, args);
    if(len < 0) return len;

    for(uint8_t i = 0; i < len; i++)
        uart_putchar(buffer[i]);

    return len;
}

int main(void)
{
    char* msg = "*E*C*O*L*E* *P*O*L*Y*T*E*C*H*N*I*Q*U*E*";
    char buf[FLASH_BUFFER_SIZE];
    Memoire24CXXX flash;

    /* initialise UART communication */
    uart_init();

#ifdef TESTING
    /* on reset la mémoire */
    for(uint16_t i = 0; i <= 0xff; i++) {
        flash.ecriture(i, 0);
        _delay_ms(5);
    }

    /* on écrit notre message */
    flash.ecriture(0x0, (uint8_t*) msg, strlen(msg));
#endif

    /* on dump la mémoire */
    uart_printf("\n\r");   
    for(uint16_t addr = 0x0; addr <= 0xffff; addr += sizeof(buf)) {
        flash.lecture(addr, (uint8_t*) buf, sizeof(buf));

#ifdef DUMP_MEM
        uart_printf("%02p\t", addr);
        for(uint8_t i = 0; i < sizeof(buf); ++i)
            uart_printf("%c ", buf[i]);  
        uart_printf("\n\r");
#else
        for(uint8_t i = 0; i < sizeof(buf); ++i)
            uart_printf("%c ", buf[i]);  
#endif
    }

    return 0;
}
