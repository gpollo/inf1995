#include <uart.h>
#include <memory.h>
#include <timer.h>
#include <adc.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <del.h>
#include <moteur.h>
//#include <sensor.h>
#include "message.h"

#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1
#define DELAY_BOUNCE 30
#define LED_OFF 0x03
#define SPEED_STATIONARY 0
#define SPEED_ROTATION (vitesse*255)/100

void nom_robot(void){
	/* On écrit le nom du robot */
	uart_putchar(MSG_NUMERO_SECTION);
    uart_printf("FireFightingB\0");
}

void numero_equipe (void){
    /* On écrit le numéro de l'équipe (9 octets) */
	uart_putchar(MSG_NUMERO_EQUIPE);
    uart_printf("6266     ");
}
	
void numero_section(void){
	/* On écrit la section (1 octet) */
	uart_putchar(MSG_NUMERO_SECTION);
	uart_putchar(3);
}

void session(void){
	/* On écrit la session (4 octets)*/
	uart_putchar(MSG_SESSION);
    uart_printf("18-1");
} 
 
void couleur_base(void){
    /* On écrit la couleur de la base (1 octet) */
    uart_putchar(MSG_COULEUR_BASE);
    uart_putchar('1');
}

void interruption_init(void) {
	
	/* Il bloque toutes les interruptions */
    cli();
	
    /* on active l'interruption INT0 */
    EIMSK |= (1 << INT0);

    /* on configure sensiblité de l'interruption aux changements  */
    EICRA |= ISC00;
    EICRA |= ISC01;
    
	/* Il permet de recevoir à nouveau des interruptions*/
    sei();
}

    /* On assume que le bouton est relâché (0x01) */
    volatile uint8_t bouton = BUTTON_RELEASED;
    
ISR(INT0_vect) {
    
    /* Il bloque toutes les interruptions */
    cli();
    
    /*Il permet de vérifier si le bouton est enfoncé (0x00) ou relâché */
    bouton = (bouton == BUTTON_PRESSED) ? 1 : 0;
    
    /* État du bouton Interrupt (1 octet) */
    uart_putchar(MSG_ETAT_INTERRUPT);
    uart_putchar(bouton);
    
    /* Le delai du anti-rebond lorsque le bouton est appuyé */
    _delay_ms(DELAY_BOUNCE);
    
    /* La fonction est finie et l'Interrupt Flag est réinitialisé à 1   */
    EIFR |= (1 << INTF0);
    
    /* Il permet de recevoir à nouveau des interruptions*/
    sei();
}


void distance_gauche(void) {
    /* Distance en cm détectée par le capteur gauche (1 octet) */
}


void distance_droite(void) {
	/* Distance en cm détectée par le capteur droite (1 octet) */
}


void couleur_del(uint8_t couleur){
	switch (couleur){
	/*Si on reçoit la donnée 0, la del est éteint */
	case MSG_DEL_ETEINT:
		del_off(LED_OFF);
		break;
	/*Si on reçoit la donnée 1, la del est verte */
	case MSG_DEL_VERTE:
		del_green();
		break;
	/*Si on reçoit la donnée 2, la del est rouge */
	case MSG_DEL_ROUGE:
		del_red();
		break;
	}
}

void moteur_controller_gauche(int8_t vitesse) {
    if(vitesse < SPEED_STATIONARY) {
        /* On met les directions vers l'arrière */
        PORTB |= (1<<2);
        vitesse = -vitesse;
    } else {
        /* On met les directions vers l'avant */
        PORTB &=~ (1<<2);
    }

    /* On active la vitesse pour roue de droite seulement */
    OCR0A = SPEED_ROTATION;
}

void moteur_controller_droite(int8_t vitesse) {
    if(vitesse < 0) {
        /* On met les directions vers l'arrière */
        PORTB |= (1<<5);
        vitesse = -vitesse;
    } else {
        /* On met les directions vers l'avant */
        PORTB &=~ (1<<5);
    }

    /* On active la vitesse pour roue de droite seulement */
    OCR0B = SPEED_ROTATION;
}

void requete_info(uint8_t identification){
    
    if(identification == MSG_ENVOIE_INFO){
        /* Le robot envoye les infos d'identification au logiciel */
        nom_robot();
        numero_equipe();
        numero_section();
        session();
        couleur_base();
   }
}

void listen(void) {
   
     /*Selon les données reçues */
	while(1) {
		switch(uart_receive()) {
			case MSG_VITESSE_GAUCHE: 
            /*Le robot exécutera une rotation à la roue gauche */
			moteur_controller_gauche(uart_receive());
			break;
			case MSG_VITESSE_DROITE:
            /*Le robot exécutera une rotation à la roue droite */
			moteur_controller_droite(uart_receive());
			break;
			case MSG_COULEUR_DEL:
            /*Le robot changera la couleur de sa del libre */
			couleur_del(uart_receive());
			break;
			case MSG_REQUETE_INFO:
            /*Le robot envoie ses informations sur le logiciel */
            requete_info(uart_receive());
			break;
		}
	}
}

int main(void) {
	uart_init();
	del_init();
	moteur_init();	
    interruption_init();
    listen();

}

