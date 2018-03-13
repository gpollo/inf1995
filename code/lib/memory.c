#include <util/twi.h>
#include <math.h>
#include <memory.h>

/**
 * Cette macro calcule l'adresse de la puce mémoire. Il est possible d'avoir
 * jusqu'à 8 puces identiques sur le même bus I2C/TWI. L'adresse d'une puce est
 * définit comme suit :
 *
 *      | ADDR7 | ADDR6 | ADDR5 | ADDR4 | ADDR3 | ADDR2 | ADDR1 | ADDR0 |
 *
 * Les bits 7 à 4 contiennent toujours la valeur 0xA (0b1010). Les bits 3 à 1
 * définissent quelle puce accèder sur le bus. La valeur maximal étant 8. Le
 * bit 0 est à 1 pour lire et 0 pour écrire.
 *
 * @param n Le numéro de la puce (0 à 7).
 * @param m Le mode d'accès, 0 pour écriture et 1 pour lecture.
 *
 * @return L'adresse de la puce mémoire.
 */
#define GET_ADDR(n,m) ((0xA0 | (n << 1)) + m)

/** Le bit de controle lorsqu'on commence/poll la communication. */
#define CONTROL_START 0

/** Le bit de controle lorsqu'on désire écrire sur la puce. */
#define CONTROL_WRITE 0

/** Le bit de controle lorsqu'on désire lire sur la puce. */
#define CONTROL_READ  1

/** La taille maximale des données que l'on peut envoyer. */
#define PAGE_SIZE 64

/** Cette macro retourne les 5 bits dédiés au statut de l'interface TWI. */
#define GET_STATUS (TWSR & 0xF8)

/** Cette macro détermine si l'adresse fut envoyé et un ACK fut reçu. */
#define SLA_W_TRANSMITTED_ACK (GET_STATUS & 0x18)

/**
 * Cette fonction bloquante est utilisée pour attendre l'écriture interne des
 * données dans la puce. En effet, la puce n'écrit rien en mémoire tant et
 * aussi longtemps que la transmission des données à écrire n'est pas fini. Par
 * conséquent, il est possible qu'elle n'ait pas finit d'écrire les données
 * lors d'une opération subséquente.
 *
 * @param n Le numéro de la puce (0 à 7).
 */
inline void ack_polling(uint8_t puce);

/**
 * Cette fonction envoit l'entête d'une communication à le puce. Selon les
 * spécifications de la mémoire 24LC256, les opérations de lecture ou d'écriture
 * sont de la forme suivante :
 *
 *      1. condition de départ
 *      2. octet de controle contenant en partie l'adresse de la puce
 *      3. ACK
 *      4. poids fort de l'adresse mémoire
 *      5. ACK
 *      6. poids faible de l'adresse mémoire
 *      7. ACK
 *      8. condition de départ
 *
 * Après cet en-tête, il y a les données propres à l'écriture ou la lecture.
 *
 * @param puce Le numéro de la puce (0 à 7).
 * @param addr L'adresse à accèder dans la mémoire.
 * @param mode Le mode dans l'octet de controle.
 */
inline void send_header(uint8_t puce, uint16_t addr, uint8_t mode);

/**
 * Cette fonction termine une communication à la puce.
 *
 * @param puce Le numéro de la puce (0 à 7).
 */
inline void send_footer();

/**
 * Cette fonction envoit 64 octets à la puce en écriture. Puisque la puce a un
 * tampon temporaire de mémoire de 64 octets, la longueur maximale d'une page
 * que l'on peut écrire est de 64 octets. Par conséquent, il faut envoyer
 * plusieurs pages de 64 octets pour écrire des grandes plages d'adresses.
 *
 * Note: Il est possible d'envoyer des données ayant une longueur supérieure à
 *       64 octets à cette fonction, mais elle ne va qu'écrire au maximum 64
 *       octets.
 *
 * @param puce Le numéro de la puce (0 à 7).
 * @param addr L'adresse d'écriture de départ.
 * @param data Un pointeur vers les données à écrire.
 * @param len  La longueur des données à écrire.
 *
 * @return La taille des données écrites.
 */
uint8_t write_page64(uint8_t puce, uint16_t addr, uint8_t* data, uint8_t len);

/* valeur par défaut du prescaler */
#ifndef TWI_PRESCALER
    #define TWI_PRESCALER 1
#endif

void memory_init(void) {
    /* on configure le prescaler du TWI */
#if TWI_PRESCALER == 1
    TWSR = 0;
#elif TWI_PRESCALER == 4
    TWSR = _BV(TWPS0);
#elif TWI_PRESCALER == 16
    TWSR = _BV(TWPS1);
#elif TWI_PRESCALER == 64
    TWSR = _BV(TWPS1) | _BV(TWPS0);
#else
    #error "Prescaler invalide pour le TWI"
#endif

    /* on configure la bitrate du TWI */
    TWBR = (F_CPU / F_SCL - 16) / (2 * pow(4, TWSR));
}

uint8_t memory_read_byte(uint8_t puce, uint16_t addr) {
    /* on s'assure que la puce a finit d'écrire ses dernières données */
    ack_polling(puce);

    /* on envoit l'entête à la puce */
    send_header(puce, addr, CONTROL_START);

    /* on transmet la condition de départ */
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on transmet la code de controle (adresse de la puce en lecture) */
    TWDR = GET_ADDR(puce, CONTROL_READ);
    TWCR = _BV(TWINT) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on effectue la lecture de l'octet */
    TWCR = _BV(TWINT) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on termine la transaction */
    send_footer(puce);

    /* on retourne la valeur lu */
    return TWDR;
}

uint8_t memory_read_block(uint8_t puce, uint16_t addr, uint8_t len, uint8_t* data) {
    /* on s'assure que la puce a finit d'écrire ses dernières données */
    ack_polling(puce);

    /* on envoit l'entête à la puce */
    send_header(puce, addr, CONTROL_START);

    /* on transmet la condition de départ */
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on transmet la code de controle (adresse de la puce en lecture) */
    TWDR = GET_ADDR(puce, CONTROL_READ);
    TWCR = _BV(TWINT) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on effectue la lecture de la page */
    uint8_t received;
    for(received = 0; received < len; received++) {
        /* on effectue la lecture de l'octet */
        if(received == len-1)
            /* on envoit un NACK au dernier octet */
            TWCR = _BV(TWINT) | _BV(TWEN);
        else
            /* on envoit un ACK pour les autres octets */
            TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
        while((TWCR & _BV(TWINT)) == 0);

        /* on copie l'octet lue */
        data[received] = TWDR;
    }

    /* on termine la transaction */
    send_footer(puce);

    /* on retourne le nombre d'octets lu */
    return received;
}

void memory_write_byte(uint8_t puce, uint16_t addr, uint8_t data) {
    /* on s'assure que la puce a finit d'écrire ses dernières données */
    ack_polling(puce);

    /* on envoit l'entête à la puce */
    send_header(puce, addr, CONTROL_WRITE);

    /* on effectue l'écriture de l'octet */
    TWDR = data;
    TWCR = _BV(TWINT) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on termine la transaction */
    send_footer(puce);
}

uint8_t memory_write_block(uint8_t puce, uint16_t addr, uint8_t len, uint8_t* data) {
    uint8_t total = 0;

    do {
        /* on tente d'écrire le bloque */
        uint8_t written = write_page64(puce, addr, data, len);

        /* on incrémente le nombre total d'octets écrit */
        total += written;

        /* on calcule la prochaine adresse à écrire */
        addr += written;

        /* on calcule les prochaines données à écrire */
        data += written;

        /* on calcule combien d'octets il reste à écrire */
        len -= written;
    } while(len > 0);

    /* on retourne le nombre total d'octets écrit */
    return total;
}

void ack_polling(uint8_t puce) {
    /* on attend la fin d'un cycle d'écriture si c'est le cas */
    for(;;) {
        /* condition de départ */
        TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
        while((TWCR & _BV(TWINT)) == 0);

        /* on écrit l'adresse I2C/TWI de la puce */
        TWDR = GET_ADDR(puce, CONTROL_START);
        TWCR = _BV(TWINT) | _BV(TWEN);
        while((TWCR & _BV(TWINT)) == 0);

        /* on regarde si le cycle d'écriture fut terminé */
        if(SLA_W_TRANSMITTED_ACK) break;
    }
}

void send_header(uint8_t puce, uint16_t addr, uint8_t control) {
    /* on transmet la condition de départ */
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on transmet la code de controle */
    TWDR = GET_ADDR(puce, control);
    TWCR = _BV(TWINT) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on transmet le poids fort de l'adresse */
    TWDR = (addr >> 8);
    TWCR = _BV(TWINT) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);

    /* on transmet le poids faible de l'adresse */
    TWDR = addr;
    TWCR = _BV(TWINT) | _BV(TWEN);
    while((TWCR & _BV(TWINT)) == 0);
}

void send_footer() {
    /* transmission de la condition d'arrêt */
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

uint8_t write_page64(uint8_t puce, uint16_t addr, uint8_t* data, uint8_t len) {
    /* on ajuste la page afin d'éviter le repliement dans l'écriture des données */
    uint16_t addr_end;
    if((addr + len) < (addr | (PAGE_SIZE-1)))
        addr_end = addr + len;
    else
        addr_end = (addr | (PAGE_SIZE -1)) + 1;
    len = addr_end - addr;

    /* on s'assure que la puce a finit d'écrire ses dernières données */
    ack_polling(puce);

    /* on envoit l'entête à la puce */
    send_header(puce, addr, CONTROL_WRITE);

    /* on effectue l'écriture de la page */
    uint8_t written;
    for(written = 0; written < len; written++) {
        /* on effectue l'écriture de l'octet */
        TWDR = data[written];
        TWCR = _BV(TWINT) | _BV(TWEN);
        while((TWCR & _BV(TWINT)) == 0);
    }

    /* on termine la transaction */
    send_footer(puce);

    /* on retourne le nombre d'octets écrit */
    return written;
}
