#define F_CPU 8000000
#define BAUD 2400

#include <avr/io.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include <stdio.h>

/* the buffer shouldn't be bigger than 255 */
#define BUFFER_SIZE 255
char buffer[BUFFER_SIZE];

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

    int8_t len = vsnprintf(buffer, BUFFER_SIZE, format, args);
    if(len < 0) return len;

    for(uint8_t i = 0; i < len; i++)
        uart_putchar(buffer[i]);

    return len;
}

int main(void)
{
    /* initialise UART communication */
    uart_init();

    /* test cases */
    uart_printf("\r");
    uart_printf("main() location: %p\n\r", &main);
    uart_printf("number: %x%x\n\r", 0xdead, 0xbeef);

    for(uint8_t i = 0;;i++) {
        uart_printf("%d\n\r", i);
        _delay_ms(1000);
    }

    return 0;
}
