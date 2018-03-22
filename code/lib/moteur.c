#include <avr/io.h>
#include <moteur.h>
#include <util/delay.h>
#include <mpu6050.h>

/* valeur par défaut du prescaler */
#ifndef MOTEUR_PRESCALER
    #define MOTEUR_PRESCALER 64
#endif

void moteur_init() {
    /* On réinitialise les registres impliqués */
    TCCR0A = 0;
    TCCR0B = 0;

    /* On s'assure que les roues sont arrêtées */
    moteur_arreter();

    /* clear au match du compteur et set au bas du compteur*/
    TCCR0A = _BV(COM0A1) | _BV(COM0B1);

    /* on configure le compteur en fast PWM */
    TCCR0A |= _BV(WGM01) | _BV(WGM00);

    /* on configure le prescaler des moteurs */
#if   MOTEUR_PRESCALER == 1
    TCCR0B = _BV(CS00);
#elif MOTEUR_PRESCALER == 8
    TCCR0B = _BV(CS01);
#elif MOTEUR_PRESCALER == 64
    TCCR0B = _BV(CS01) | _BV(CS00);
#elif MOTEUR_PRESCALER == 256
    TCCR0B = _BV(CS02);
#elif MOTEUR_PRESCALER == 1024
    TCCR0B = _BV(CS02) | _BV(CS00);
#else
    #error "Invalid prescaled value"
#endif

    /* on configure les pins de PWM sortie */
    OC0A_DDR |= _BV(OC0A_BIT);
    OC0B_DDR |= _BV(OC0B_BIT);
    DDRB = 0xff;    
}

void moteur_avancer(uint8_t speed) {
    /* On met les directions vers l'avant */
    PORTB &=~ (1<<2);
    PORTB &=~ (1<<5);

    /* On active la vitesse */
    OCR0A = speed;
    OCR0B = speed;
}

void moteur_reculer(uint8_t speed) {
    /* On met les directions vers l'arriere */
    PORTB |= (1<<2);
    PORTB |= (1<<5);

    /* On active la vitesse */
    OCR0A = speed;
    OCR0B = speed;
}

void moteur_arreter() {
    /* On configure les pins du PWM en sortie */
    OCR0A = 0;
    OCR0B = 0;
}

void moteur_tourner_droite() {
    /* On met les directions vers l'avant */
    PORTB &=~ (1<<2);
    PORTB &=~ (1<<5);

    /* On active la vitesse pour roue de gauche seulement */
    OCR0A = 0;
    OCR0B = ROTATION_SPEED;

    _delay_ms(2300);
    moteur_arreter();
}

void moteur_tourner_gauche() {
    /* On met les directions vers l'avant */
    PORTB &=~ (1<<2);
    PORTB &=~ (1<<5);

    /* On active la vitesse pour roue de droite seulement */
    OCR0A = ROTATION_SPEED;
    OCR0B = 0;

    _delay_ms(2300);
    moteur_arreter();
}

void surplace(int16_t speed) {
    if(speed < 0) {
        /* on met les directions vers la gauche */
        PORTB |=  (1<<2);
        PORTB &= ~(1<<5);

        OCR0A = (-speed)%255;
        OCR0B = (-speed)%255;
    } else {
        /* on met les directions vers la droite */
        PORTB &= ~(1<<2);
        PORTB |=  (1<<5);

        OCR0A = (speed)%255;
        OCR0B = (speed)%255;
    }
}

#define Kp (50)
#define Kd (-0)

#include <uart.h>

void moteur_suivre(float angle) {
    float theta = 0;

    struct gyroscope gyro1;
    struct gyroscope gyro2;

    uint16_t time1, time2;

    uint8_t turn = 0;

    mpu6050_read_gyroscope(0, &gyro1);
    time1 = TCNT1;

    _delay_ms(1);

    while(1) {
        float delta;

        if(turn) {
            mpu6050_read_gyroscope(0, &gyro1);
            time1 = TCNT1;

            if(time1 < time2)
                delta = (0xFFFF-time2)+time1;
            else
                delta = time1-time2;
        } else {
            mpu6050_read_gyroscope(0, &gyro2);
            time2 = TCNT1;

            if(time2 < time1)
                delta = (0xFFFF-time1)+time2;
            else
                delta = time2-time1;
        }


        turn = ~turn;

        delta = delta*1024;



        float area = ((gyro1.x/131+gyro2.x/131)*delta)/2;
        theta += area/(F_CPU);

        float error = angle-theta;

        if(-2 < error && error < 2) {
            moteur_arreter();
            continue;
        }

        float control = (Kp*error+Kd*(gyro1.x/131+gyro2.x/131))/2;

        surplace((int16_t) control);


        uart_printf("%d\n\r", (int16_t) control);


    }
}
