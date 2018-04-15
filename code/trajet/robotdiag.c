#include <stdlib.h>
#include <util/delay.h>
#include <uart.h>
#include <timer.h>
#include <adc.h>
#include <interrupt.h>
#include <del.h>
#include <moteur.h>
#include <sensor.h>

#include "message.h"
#include "robotdiag.h"

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

void send_nom_robot(void) {
    /* on écrit le nom du robot (13 octets) */
    uart_putchar(MSG_NOM_ROBOT);
    uart_printf(NOM_ROBOT);
}

void send_numero_equipe(void) {
    /* on écrit le numéro de l'équipe (9 octets) */
    uart_putchar(MSG_NUMERO_EQUIPE);
    uart_printf(NUMERO_EQUIPE);
}
	
void send_numero_section(void) {
    /* on écrit la section (1 octet) */
    uart_putchar(MSG_NUMERO_SECTION);
    uart_putchar(NUMERO_SECTION);
}

void send_session(void) {
    /* on écrit la session (4 octets) */
    uart_putchar(MSG_SESSION);
    uart_printf(SESSION);
} 
 
void send_couleur_base(void) {
    /* on écrit la couleur de la base (1 octet) */
    uart_putchar(MSG_COULEUR_BASE);
    uart_putchar(COULEUR_BASE);
}

void send_capteurs(struct capteurs* capteurs) {
    /* on obtient la distance des capteurs en centimètres (1 octet) */
    uint8_t distanceG = (uint8_t) ((capteurs->gauche.value) / 10);
    uint8_t distanceD = (uint8_t) ((capteurs->droit.value) / 10);

    /* on envoie la distance du capteur gauche */
    uart_putchar(MSG_DISTANCE_GAUCHE);
    uart_putchar(distanceG);

    /* on envoie la distance du capteur droit */
    uart_putchar(MSG_DISTANCE_DROIT);
    uart_putchar(distanceD);
 }   

void control_del(uint8_t couleur) {
    switch (couleur) {
    /* on ferme la del */
	case MSG_DEL_ETEINT:
	    del_off(LED_OFF);
	    break;

    /* on met la del verte */
	case MSG_DEL_VERTE:
	    del_green();
	    break;

    /* on met la del rouge */
	case MSG_DEL_ROUGE:
	    del_red();
	    break;
    }
}

void send_info() {
    /* on envoie le nom du robot */
    send_nom_robot();

    /* on envoit le numéro de l'équipe */
    send_numero_equipe();

    /* on envoit le numéro de section */
    send_numero_section();

    /* on envoit la session */
    send_session();

    /* on envoit la couleur de la base */
    send_couleur_base();
}

void listen(void) {
    /* on obtient l'ID du message */
    uint8_t id =  uart_receive();

    /* on obtient le paramètre du message */
    uint8_t data = uart_receive();

    switch(id) {
    /* on controle la roue gauche */
    case MSG_VITESSE_GAUCHE: 
        moteur_controler_gauche(data);
        break;

    /* on controle la roue droite */
    case MSG_VITESSE_DROITE:
        moteur_controler_droite(data);
        break;

    /* on change la couleur de la del */
    case MSG_COULEUR_DEL:
        control_del(data);
        break;

    /* on envoit les données sur le robot */
    case MSG_REQUETE_INFO:
        send_info();
        break;
    }
}

void button_callback(uint8_t bouton, void* data) {
    /* cette variable est inutilisée */
	UNUSED(data);
	
    /* on écrit l'état du bouton (1 octet) */
    uart_putchar(MSG_ETAT_INTERRUPT);
    uart_putchar(bouton);
}

void capteurs_callback(void* data) {
    /* cette variable est inutilisée */
    UNUSED(data);

    /* on initialise les capteurs */
    static struct capteurs capteurs =  CAPTEURS_INIT(0,1);

    /* on lit les deux capteurs */
    sensor_read(&capteurs);

    /* on obtient la distance des capteurs */
    sensor_get_value(&(capteurs.gauche));
    sensor_get_value(&(capteurs.droit));

    /* on affiche la distance dans le logiciel */
    send_capteurs(&capteurs);
}

struct callback capteur_callback = {
    .func = &capteurs_callback,
    .time = DELAY_SENSOR,
    .repeat = REPEAT_1,
    .data = NULL,
};

void robotdiag_main(void) {
    /* pour la communication UART */
    uart_init();

    /* pour le controle de la del */
    del_init();

    /* pour le controle des moteurs */
    moteur_init();

    /* pour la lecture des senseurs */
    adc_init();

    /* on active le bouton en interruption */
    interruption_init(&button_callback, NULL);

    /* pour les timers */
    timer_init();

    /* on démarre le timer qui envoit les capteurs */
    timer_start(&capteur_callback, NULL);

    /* on écoute ce que le logiciel envoit */
    while(1) listen();
}
