#ifndef INCLUDE_UART_H
#define INCLUDE_UART_H

#ifdef _cplusplus
extern "C" {
#endif

/**
 * Cette fonction initialise la transmission et la réception sur le port UART0
 * du microcontrolleur.
 */
void uart_init(void);

/**
 * Cette fonction envoit un octet sur la ligne UART0 du microcontrolleur.
 *
 * @param octet L'octet à envoyer.
 */
void uart_putchar(char octet);

/**
 * Cette fonction envoit une chaîne formattée de caractères sur le port UART0
 * du microcontrolleur.
 *
 * NOTE: La taille de la chaîne de caractères formattée ne peut pas dépasser
 *       une valeur de 255 caractères.
 *
 * @param format Le format de la chaîne de caractères.
 * @param ...    Les valeurs de la chaîne de caratères.
 *
 * @return La taille de la chaîne envoyée si possible, sinon un nombre négatif.
 */
int8_t uart_printf(char* format, ...);

#ifdef _cplusplus
}
#endif

#endif
