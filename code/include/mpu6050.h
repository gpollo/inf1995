#ifndef INCLUDE_MPU6050_H
#define INCLUDE_MPU6050_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This structure defines the gyroscope measurements in the order of the chip's
 * registers.
 */
struct gyroscope {
    /** The x measurement value. */
    int16_t x;

    /** The y measurement value. */
    int16_t y;

    /** The z measurement value. */
    int16_t z;
} __attribute__((packed));

/**
 * This structure defines the accelerometer measurements in the order of the
 * chip's registers.
 */
struct accelerometer {
    /** The x measurement value. */
    int16_t x;

    /** The y measurement value. */
    int16_t y;

    /** The z measurement value. */
    int16_t z;
} __attribute__((packed));

/**
 * This structure defines the measurements that can be read from the chip in
 * the order of the chip's registers.
 */
struct measurements {
    /** The gyroscope measurements. */
    struct gyroscope gyroscope;

    /** The temperature measurement. */
    int16_t temperature;

    /** The accelerometer measurements. */
    struct accelerometer accelerometer;
} __attribute__((packed));

/**
 * Cette fonction initialise le bus I2C/TWI relié au MPU-6050.
 */
void mpu6050_init(void);

/**
 * Cette fonction lit plusieurs registres de la puce.
 *
 * @param puce  Le numéro de la puce (0 ou 1).
 * @param reg   Le registre de départ à lire de la puce.
 * @param count Le nombre de registres à lire.
 * @param data  Le tampon dans lequel on met les données lues.
 */
void mpu6050_read_registers(uint8_t puce, uint8_t reg, uint8_t count, uint8_t* data);

/**
 * Cette fonction lit un seul registre de la puce.
 *
 * @param puce Le numéro de la puce (0 ou 1).
 * @param reg  Le registre à lire de la puce.
 *
 * @return La valeur lut du registre.
 */
uint8_t mpu6050_read_register(uint8_t puce, uint8_t reg);

/**
 * Cette fonction écrit plusieurs registres dans la puce.
 *
 * @param puce  Le numéro de la puce (0 ou 1).
 * @param reg   Le registre de départ à écrire dans la puce.
 * @param count Le nombre de registres à écrire.
 * @param data  Les données à écrire dans les registres.
 */
void mpu6050_write_registers(uint8_t puce, uint8_t reg, uint8_t count, uint8_t* data);

/**
 * Cette fonction écrit un seul registre dans la puce.
 *
 * @param puce Le numéro de la puce (0 ou 1).
 * @param reg  Le registre à écrire dans la puce.
 * @param data La valeur à écrire dans le registre.
 */
void mpu6050_write_register(uint8_t puce, uint8_t reg, uint8_t data);

/** The register for the configuration of the gyroscope. */
#define REG_GYRO_CONFIG 0x1B
#define XG_ST   7 /** The bit for self-testing the x gyroscope. */
#define YG_ST   6 /** The bit for self-testing the y gyroscope. */
#define ZG_ST   5 /** The bit for self-testing the z gyroscope. */
#define FS_SEL1 4 /** The second bit for selecting the range. */
#define FS_SEL0 3 /** The first bit for selecting the range. */

/** The register for the configuration of the accelerometer. */
#define REG_ACCEL_CONFIG 0x1C
#define XA_ST    7 /** The bit for self-testing the x gyroscope. */
#define YA_ST    6 /** The bit for self-testing the y gyroscope. */
#define ZA_ST    5 /** The bit for self-testing the z gyroscope. */
#define AFS_SEL1 4 /** The second bit for selecting the range. */
#define AFS_SEL0 3 /** The first bit for selecting the range. */

/** The register for the higher byte of the x acceleration value. */
#define REG_ACCEL_XOUTH 0x3B

/** The register for the lower byte of the x acceleration value. */
#define REG_ACCEL_XOUTL 0x3C

/** The register for the higher byte of the y acceleration value. */
#define REG_ACCEL_YOUTH 0x3D

/** The register for the lower byte of the y acceleration value. */
#define REG_ACCEL_YOUTL 0x3E

/** The register for the higher byte of the z acceleration value. */
#define REG_ACCEL_ZOUTH 0x3F

/** The register for the lower byte of the z acceleration value. */
#define REG_ACCEL_ZOUTL 0x40

/** The register for the higher byte of temperature value. */
#define REG_TEMP_OUTH 0x41

/** The register for the lower byte of temperature value. */
#define REG_TEMP_OUTL 0x42

/** The register for the higher byte of the x gyroscope value. */
#define REG_GYRO_XOUTH 0x43

/** The register for the lower byte of the x gyroscope value. */
#define REG_GYRO_XOUTL 0x44

/** The register for the higher byte of the y gyroscope value. */
#define REG_GYRO_YOUTH 0x45

/** The register for the lower byte of the y gyroscope value. */
#define REG_GYRO_YOUTL 0x46

/** The register for the higher byte of the z gyroscope value. */
#define REG_GYRO_ZOUTH 0x47

/** The register for the lower byte of the z gyroscope value. */
#define REG_GYRO_ZOUTL 0x48

#ifdef __cplusplus
}
#endif

#endif
