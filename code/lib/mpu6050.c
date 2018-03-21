#include <util/twi.h>
#include <mpu6050.h>
#include <math.h>
#include <utils.h>

    #define DEBUG
#include <uart.h>

#define GET_CONTROL(puce, op) ((0b11010000) | (puce << 1) | (op))

/* valeur par défaut du prescaler */
#ifndef TWI_PRESCALER
    #define TWI_PRESCALER 1
#endif

/* valeur minimale de la moyenne pour calibrer le senseur */
#ifndef MEAX_MIN
    #define MEAN_MIN (-30)
#endif

/* valeur maximale de la moyenne pour calibrer le senseur */
#ifndef MEAN_MAX
    #define MEAN_MAX ( 30)
#endif

/* valeur par défaut pour l'offset en X du gyroscope */
#ifndef GYRO_OFFS_X
    #define GYRO_OFFS_X 14
#endif

/* valeur par défaut pour l'offset en Y du gyroscope */
#ifndef GYRO_OFFS_Y
    #define GYRO_OFFS_Y 125
#endif

/* valeur par défaut pour l'offset en Z du gyroscope */
#ifndef GYRO_OFFS_Z
    #define GYRO_OFFS_Z 138
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

void mpu6050_read_gyroscope(uint8_t puce, struct gyroscope* gyro) {
    /* on lit les données du gyroscope */
    mpu6050_read_registers(puce, REG_GYRO_XOUTH, sizeof(*gyro), (uint8_t*) gyro);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* on convertit l'ordre des octets si nécessaire */
    gyro->x = FLIP_WORD(gyro->x);
    gyro->y = FLIP_WORD(gyro->y);
    gyro->z = FLIP_WORD(gyro->z);
#endif
}

void mpu6050_set_gyroscope_offset(uint8_t puce, struct offset* offset) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* on convertit l'ordre des octets si nécessaire */
    offset->x = FLIP_WORD(offset->x);
    offset->y = FLIP_WORD(offset->y);
    offset->z = FLIP_WORD(offset->z);
#endif

    mpu6050_write_registers(puce, REG_GYRO_XOFFSH, sizeof(*offset), (uint8_t*) offset);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* on convertit l'ordre des octets si nécessaire */
    offset->x = FLIP_WORD(offset->x);
    offset->y = FLIP_WORD(offset->y);
    offset->z = FLIP_WORD(offset->z);
#endif
}

void mpu6050_set_gyroscope_offset_default(uint8_t puce) {
    /* on utilise les valeurs par défauts */
    struct offset offset = {
        .x = GYRO_OFFS_X,
        .y = GYRO_OFFS_Y,
        .z = GYRO_OFFS_Z,
    };

    /* on calibre le gyroscope */
    mpu6050_set_gyroscope_offset(puce, &offset);
}

void mpu6050_calibrate_gyroscope(uint8_t puce) {
    struct gyroscope gyro;

    struct offset offset;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;

    mpu6050_read_gyroscope(puce, &gyro);
    float meanx = gyro.x;
    float meany = gyro.y;
    float meanz = gyro.z;

    uint8_t count = 0;
    while(1) {
        mpu6050_set_gyroscope_offset(puce, &offset);

        mpu6050_read_gyroscope(puce, &gyro);

        uart_debug("%d | %6d %6d %6d | %6d %6d %6d | %6f %6f %6f\r",
            count,
            gyro.x, gyro.y, gyro.z,
            offset.x, offset.y, offset.z,
            meanx, meany, meanz);

        meanx = (meanx + gyro.x) / 2;
        meany = (meany + gyro.y) / 2;
        meanz = (meanz + gyro.z) / 2;

        if(meanx < 0)
            offset.x++;
        else
            offset.x--;

        if(meany < 0)
            offset.y++;
        else
            offset.y--;

        if(meanz < 0)
            offset.z++;
        else
            offset.z--;

        if(!(MEAN_MIN < meanx && meanx < MEAN_MAX)) {
            count = 0;
            continue;
        }

        if(!(MEAN_MIN < meany && meany < MEAN_MAX)) {
            count = 0;
            continue;
        }

        if(!(MEAN_MIN < meanz && meanz < MEAN_MAX)) {
            count = 0;
            continue;
        }

        count++;

        if(count > 100) break;
    }

    uart_printf("\n\rcalibration values: %d %d %d\n\r", offset.x, offset.y, offset.z);
}
