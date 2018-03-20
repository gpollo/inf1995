#include <util/twi.h>
#include <mpu6050.h>
#include <math.h>

#define GET_CONTROL(puce, op) ((0b11010000) | (puce << 1) | (op))

/* valeur par défaut du prescaler */
#ifndef TWI_PRESCALER
    #define TWI_PRESCALER 1
#endif

void mpu6050_init(void) {
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

void mpu6050_read_registers(uint8_t puce, uint8_t reg, uint8_t count, uint8_t* data) {
    /* on transmet la condition de départ */
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);

    /* on transmet le code de contrôle */
    TWDR = GET_CONTROL(puce, TW_WRITE);
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);

    /* on attend le ACK  */
    while(TW_STATUS != TW_MT_SLA_ACK);

    /* on transmet l'adresse du registre */
    TWDR = reg;
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);

    /* on attend le ACK  */
    while(TW_STATUS != TW_MT_DATA_ACK);

    /* on transmet la condition de départ */
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);

    /* on transmet le code de contrôle */
    TWDR = GET_CONTROL(puce, TW_READ);
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);

    /* on attend le ACK  */
    while(TW_STATUS != TW_MR_SLA_ACK);

    /* on effectue la lecture de plusieurs registres */
    for(uint8_t received = 0; received < count ; received++) {
        if(received == count-1)
            /* on envoit un NACK pour le dernier octet */
            TWCR = _BV(TWINT) | _BV(TWEN);
        else
            /* on envoit un ACK pour les autres octet */
            TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
        loop_until_bit_is_set(TWCR, TWINT);

        /* on copie l'octet lue */
        data[received] = TWDR;
    }

    /* on transmet la condition d'arrêt */
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

uint8_t mpu6050_read_register(uint8_t puce, uint8_t reg) {
    /* on lit seulement un octet */
    uint8_t data;
    mpu6050_read_registers(puce, reg, 1, &data);
    return data;
}

void mpu6050_write_registers(uint8_t puce, uint8_t reg, uint8_t count, uint8_t* data) {
    /* on transmet la condition de départ */
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);

    /* on transmet le code de contrôle */
    TWDR = GET_CONTROL(puce, TW_WRITE);
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);

    /* on attend le ACK  */
    while(TW_STATUS != TW_MT_SLA_ACK);
    
    /* on transmet l'adresse du registre */
    TWDR = reg;
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);

    /* on attend le ACK  */
    while(TW_STATUS != TW_MT_DATA_ACK );

    /* on effectue la lecture de plusieurs registres */
    for(uint8_t sent = 0; sent < count ; sent++) {
        /* on transmet un octet */
        TWDR = data[sent];
        TWCR = _BV(TWINT) | _BV(TWEN);
        loop_until_bit_is_set(TWCR, TWINT);

        /* on attend le ACK  */
        while(TW_STATUS != TW_MT_DATA_ACK);
    }

    /* on transmet la condition d'arrêt */
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

void mpu6050_write_register(uint8_t puce, uint8_t reg, uint8_t data) {
    /* on écrit seulement un octet */
    mpu6050_write_registers(puce, reg, 1, &data);
}

void mpu6050_set_sleep(uint8_t puce, uint8_t mode) {
    /* on lit la valeur actuelle du registre */
    uint8_t data = mpu6050_read_register(puce, REG_PWR_MGMT_1);

    /* on change le bit */
    if(mode)
        data |= _BV(BIT_SLEEP);
    else
        data &= ~_BV(BIT_SLEEP);

    /* on écrit la nouvelle valeur dans le régistre */
    mpu6050_write_register(puce, REG_PWR_MGMT_1, data);
}
