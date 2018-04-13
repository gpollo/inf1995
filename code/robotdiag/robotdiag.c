#include <uart.h>
#include <memory.h>
#include <timer.h>
#include <adc.h>
#include <util/delay.h>
#include <interrupt.h>
#include <stdlib.h>
#include <del.h>
#include <moteur.h>
#include <sensor.h>
#include <timer.h>
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

/** Cette macro définie le delai pour l'affichage du capteur. */
#define DELAY_SENSOR 100

/** Cette macro définie que le timer est répété une seule fois. */
#define REPEAT_1 1

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
    /* on écrit la session (4 octets) */
    uart_putchar(MSG_SESSION);
    uart_printf(SESSION);
} 
 
void couleur_base(void) {
    /* on écrit la couleur de la base (1 octet) */
    uart_putchar(MSG_COULEUR_BASE);
    uart_putchar(COULEUR_BASE);
}

void button(uint8_t bouton, void* data) {
	UNUSED(data);
	
    /* on écrit l'état du bouton (1 octet) */
    uart_putchar(MSG_ETAT_INTERRUPT);
    uart_putchar(bouton);
}

void distance_capteur(struct capteurs* capteurs) {
    /* on désire les distances en cm détectées par le capteur gauche et droit (1 octet) */
    uint8_t distanceG, distanceD;
    distanceG = (uint8_t) ((capteurs->gauche.value) / 10);
    distanceD = (uint8_t) ((capteurs->droit.value) / 10);
    /* on affiche la distance du capteur gauche */
    uart_putchar(MSG_DISTANCE_GAUCHE);
    uart_putchar(distanceG);
    /*on affiche la distance du capteur droit */
    uart_putchar(MSG_DISTANCE_DROIT);
    uart_putchar(distanceD);
 }   

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
}

void listen(void) {
    /* selon les données reçues */
    while(1) {        
        uint8_t id =  uart_receive();
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

void send_capteurs(void* data) {
    /* on initialise les capteurs */	
    struct capteurs capteurs =  CAPTEURS_INIT(0,1);
    /* on lit les deux capteurs */
    sensor_read(&capteurs);
    /* on obtient la distance des capteurs */
    sensor_get_value(&(capteurs.gauche));
    sensor_get_value(&(capteurs.droit));
    /* on affiche la distance dans le logiciel */
    distance_capteur(&capteurs);
}

struct callback capteur_callback = {
    /* la fonction à appeler */
    .func = &send_capteurs,
    /* le temps d'attente en millisecondes */
    .time = DELAY_SENSOR,
    /* le nombre de fois que le timer doit être répété */
    .repeat = REPEAT_1,
    /* aucun pointeur à envoyer à la fonction */
    .data = NULL,
};

int main(void) {
    uart_init();
    del_init();
    moteur_init();	
    interruption_init(&button, NULL);
    adc_init();
    timer_init();
    timer_start(&capteur_callback, NULL);
    listen();
}
