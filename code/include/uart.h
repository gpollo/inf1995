#ifndef INCLUDE_UART_H
#define INCLUDE_UART_H

#ifdef __cplusplus
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
 * Note: La taille de la chaîne de caractères formattée ne peut pas dépasser
 *       une valeur de 255 caractères.
 *
 * @param format Le format de la chaîne de caractères.
 * @param ...    Les valeurs de la chaîne de caratères.
 *
 * @return La taille de la chaîne envoyée si possible, sinon un nombre négatif.
 */
int8_t uart_printf(char* format, ...);

/**
 * Cette fonction reçoit les données. 
 *
 * Note: Les données ne peuvent pas dépasser 8 bits.
 *
 * @return Les données reçues provenant du buffer.
 */
uint8_t uart_receive(void);

#ifdef __cplusplus
}
#endif

#endif
