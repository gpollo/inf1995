#include <avr/io.h>
#include <util/setbaud.h>
#include <stdio.h>
#include <uart.h>

/* la taille du buffer ne peut pas être supérieure à 255 */
#define BUFFER_SIZE 255
char buffer[BUFFER_SIZE];

void uart_init(void) {
    /* on réinitialise les registres impliqués */
    UCSR0A = 0;
    UCSR0B = 0;
    UCSR0C = 0;

    /* on configure si on double la vitesse de communication */
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    /* on configure la vitesse de la communication */
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    /* on activate la réception (RX) et la transmission (TX) */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    /* on configure 8-bits par transmission */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

    /* on configure la pin de transmission en sortie */
    TXD_DDR |= _BV(TXD_BIT);
}

void uart_putchar(char octet) {
    /* on attend que le dernier octet fut envoyé */
    loop_until_bit_is_set(UCSR0A, UDRE0);

    /* on écrit le nouveau octet */
    UDR0 = octet;
}

int8_t uart_printf(char* format, ...) {
    /* on initialise la liste d'arguments variables */
    va_list args;
    va_start(args, format);

    /* on formatte la chaîne de caractères */
    int8_t len = vsnprintf(buffer, BUFFER_SIZE, format, args);
    if(len < 0) return len;

    /* on écrit chaque caractère sur la ligne */
    for(uint8_t i = 0; i < len; i++) uart_putchar(buffer[i]);

    /* on retourne le nombre de caractère écrit */
    return len;
}

uint8_t uart_receive(void) {
    /* on attend que les données sont reçues */
    loop_until_bit_is_set(UCSR0A, RXC0);

    /* prend et retourne les données reçues provenant du buffer */
    return UDR0;
}

uint8_t uart_getline(uint8_t len, uint8_t* buffer) {
    uint8_t pos = 0;

    while(pos < len-1) {
        /* on lit un octet */
        uint8_t byte = uart_receive();

        /* on renvoit le caractère pour echo */
        uart_printf("%c", byte);
        
        /* on quitte à une nouvelle ligne */
        if(byte == '\n' || byte == '\r') break;

        /* on ajoute l'octet au buffer */
        buffer[pos++] = byte;
    }

    /* on change de ligne */
    uart_printf("\n\r");

    /* on termine la ligne */
    buffer[pos] = '\0';

    /* on retourne la taille de la ligne */
    return pos;
}
