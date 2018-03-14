#ifndef INCLUDE_UART_H
#define INCLUDE_UART_H

#include <stdint.h>

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

/**
 * Cette fonction lit une ligne en entrée.
 *
 * Note: La ligne lue est terminée par un caractère null.
 *
 * @param len    La taille du buffer dans lequel on écrit la ligne.
 * @param buffer Le buffer dans lequel on écrit la ligne.
 *
 * @return La taille de la ligne lue.
 */
uint8_t uart_getline(uint8_t len, uint8_t* buffer);

#ifdef __cplusplus
}
#endif

#endif
