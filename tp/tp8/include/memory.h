#ifndef INCLUDE_MEMORY_H
#define INCLUDE_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette fonction initialise la fréquence de la communication TWI vers la puce.
 *
 * Note: La macro #TWI_PRESCALER peut être utilisée pour controller le prescaler
 *       de la fréquence de communication.
 */
void memory_init(void);

/**
 * Cette fonction lit un octet dans la mémoire à une adresse spécifiée.
 *
 * @param puce Le numéro de la puce (0 à 7).
 * @param addr L'adresse de l'octet à lire.
 *
 * @return L'octet lu dans la mémoire.
 */
uint8_t memory_read_byte(uint8_t puce, uint16_t addr);

/**
 * Cette fonction lit un bloc de mémoire à partir d'une adresse spécifiée.
 *
 * @param puce Le numéro de la puce (0 à 7).
 * @param addr L'adresse du bloc à lire.
 * @param len  La taille du bloc à lire.
 * @param data Un pointeur vers un tampon pour écrire les données reçues.
 *
 * @return La nombre d'octets lu.
 */
uint8_t memory_read_block(uint8_t puce, uint16_t addr, uint8_t len, uint8_t* data);

/**
 * Cette fonction écrit un octet dans la mémoire à une adresse spécifiée.
 *
 * @param puce Le numéro de la puce (0 à 7).
 * @param addr L'adresse de l'octet à écrire.
 * @param data L'octet à écrire.
 */
void memory_write_byte(uint8_t puce, uint16_t addr, uint8_t data);

/**
 * Cette fonction écrit un bloc de mémoire à partir d'une adresse spécifiée.
 *
 * @param puce Le numéro de la puce (0 à 7).
 * @param addr L'adresse du bloc à écrire.
 * @param len  La taille du bloc à écrire.
 * @param data Un pointeur vers les données à écrire.
 *
 * @return La nombre d'octets écrit.
 */
uint8_t memory_write_block(uint8_t puce, uint16_t addr, uint8_t len, uint8_t* data);

#ifdef __cplusplus
}
#endif

#endif
