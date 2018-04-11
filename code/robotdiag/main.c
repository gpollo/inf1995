#include <uart.h>
#include <memory.h>
#include <timer.h>
#include <adc.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <del.h>
#include <moteur.h>
#include <sensor.h>
#include "message.h"
#include <sensor_data.h>

/** Cette macro définie le nom du robot. */
#define NOM_ROBOT "FireFightingB\0"

/** Cette macro définie le numéro d'équipe. */
#define NUMERO_EQUIPE "6266     "

/** Cette macro définie le numéro de la section. */
#define NUMERO_SECTION 3

/** Cette macro définie la session. */
#define SESSION "18-1"

/** Cette macro définie la couleur de la base du robot. */
#define COULEUR_BASE '1'

/** Cette macro définie le bouton Interrupt qui est appuyé. */
#define BUTTON_PRESSED 0

/** Cette macro définie le bouton Interrupt qui est relâché */
#define BUTTON_RELEASED 1

/** Cette macro définie l'anti-rebond pour le bouton Interrupt. */
#define DELAY_BOUNCE 30

/** Cette macro définie la DEL libre est éteinte. */
#define LED_OFF 0x03

#define ID uart_receive()

#define DATA uart_receive()

void nom_robot(void) {
    /* on écrit le nom du robot (13 octets) */
    uart_putchar(MSG_NOM_ROBOT);
    uart_printf(NOM_ROBOT);
}

void numero_equipe(void) {
    /* on écrit le numéro de l'équipe (9 octets) */
    uart_putchar(MSG_NUMERO_EQUIPE);
    uart_printf(NUMERO_EQUIPE);
}
	
void numero_section(void) {
    /* on écrit la section (1 octet) */
    uart_putchar(MSG_NUMERO_SECTION);
    uart_putchar(NUMERO_SECTION);
}

void session(void) {
    /* on écrit la session (4 octets)*/
    uart_putchar(MSG_SESSION);
    uart_printf(SESSION);
} 
 
void couleur_base(void) {
    /* on écrit la couleur de la base (1 octet) */
    uart_putchar(MSG_COULEUR_BASE);
    uart_putchar(COULEUR_BASE);
}

void interruption_init(void) {
    /* on désactive les interruptions */
    cli();
	
    /* on active l'interruption INT0 */
    EIMSK |= (1 << INT0);

    /* on configure sensiblité de l'interruption aux changements */
    EICRA |= ISC00;
    EICRA |= ISC01;
    
	/* on active les interruptions */
    sei();
}

/* par défaut, on assume que le bouton est relâché */
volatile uint8_t bouton = BUTTON_RELEASED;
    
ISR(INT0_vect) {
    /* on désactive les interruptions */
    cli();
    
    /* on détermine l'état du bouton */
    bouton = (bouton == BUTTON_PRESSED) ? 1 : 0;
    
    /* on écrit l'état du bouton (1 octet) */
    uart_putchar(MSG_ETAT_INTERRUPT);
    uart_putchar(bouton);
    
    /* on applique un délais pour l'anti-rebond */
    _delay_ms(DELAY_BOUNCE);
    
    /* on réactive cet interruption */
    EIFR |= (1 << INTF0);
    
    /* on active les interruptions */
    sei();
}

//void distance_gauche(void) {
    /* distance en cm détectée par le capteur gauche (1 octet) */
 //   uart_putchar(MSG_DISTANCE_GAUCHE);
   // struct capteurs capteurs =  CAPTEURS_INIT(0,1);
    //sensor_read(&capteurs); 
    //sensor_get_value(&(capteurs.gauche));
//}

//void distance_droite(void) {
    /* distance en cm détectée par le capteur droite (1 octet) */
  //   uart_putchar(MSG_DISTANCE_DROIT);
    
    //struct capteurs capteurs = CAPTEURS_INIT(0,1);
     //sensor_read(&capteurs);
     //sensor_get_value(&(capteurs.droit));
//}

void couleur_del(uint8_t couleur) {
    switch (couleur) {
	case MSG_DEL_ETEINT:
        /* on ferme la DEL */
	    del_off(LED_OFF);
	    break;
	case MSG_DEL_VERTE:
        /* on met la DEL verte */
	    del_green();
	    break;
	case MSG_DEL_ROUGE:
        /* on met la DEL rouge */
	    del_red();
	    break;
    }
}

void requete_info() {
    /* le robot envoye les infos d'identification au logiciel */
    nom_robot();
    numero_equipe();
    numero_section();
    session();
    couleur_base();
   // distance_droite();
    //distance_gauche();
}


void listen(void) {
    /* selon les données reçues */
    while(1) {
        uint8_t id = uart_receive();
        uint8_t data = uart_receive();
            switch(id) {
	        case MSG_VITESSE_GAUCHE: 
	         /* le robot exécutera une rotation à la roue gauche */
		    moteur_controler_gauche(data);
		    break;
	        case MSG_VITESSE_DROITE:
		 /* le robot exécutera une rotation à la roue droite */
		    moteur_controler_droite(data);
		    break;
	        case MSG_COULEUR_DEL:
                /* le robot changera la couleur de sa del libre */
		    couleur_del(data);
		    break;
	        case MSG_REQUETE_INFO:
                /* le robot envoie ses informations sur le logiciel */
		    requete_info();
		    break;
        }
    }
}

int main(void) {
    uart_init();
    del_init();
    moteur_init();	
    //distance_droite();
   // distance_gauche();
    interruption_init();
    listen();
}
