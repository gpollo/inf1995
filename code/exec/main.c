
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


void nom_robot(void){
	/* On écrit le nom du robot */
	uart_putchar(0xf0);
    uart_printf("FireFightingB\0");
}

void numero_equipe (void){
    /* On écrit le numéro de l'équipe (9 octets) */
	uart_putchar(0xf1);
    uart_printf("6266     ");
}
	
void numero_section(void){
	/* On écrit le groupe (1 octet) */
	uart_putchar(0xf2);
	uart_putchar(3);
}

void session(void){
	/* On écrit la session (4 octets)*/
	uart_putchar(0xf3);
    uart_printf("18-1");
} 
 
void couleur_base(void){
    /* On écrit la couleur de la base (1 octet) */
    uart_putchar(0xf4);
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
    volatile uint8_t bouton = 1;
    
ISR(INT0_vect) {
    
    /* Il bloque toutes les interruptions */
    cli();
    
    /*Il permet de vérifier si le bouton est enfoncé (0x00) ou relâché */
    bouton = (bouton == 0) ? 1 : 0;
    
    /* État du bouton Intrerrupt (1 octet) */
    uart_putchar(0xf5);
    uart_putchar(bouton);
    
    /* Le delai du anti-rebond lorsque le bouton est appuyé */
    _delay_ms(30);
    
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
	case 0:
		del_off(0x03);
		break;
	/*Si on reçoit la donnée 1, la del est verte */
	case 1:
		del_green();
		break;
	/*Si on reçoit la donnée 2, la del est rouge */
	case 2:
		del_red();
		break;
	}
}

void moteur_controller_gauche(int8_t vitesse) {
    if(vitesse < 0) {
        /* On met les directions vers l'arrière */
        PORTB |= (1<<2);
        vitesse = -vitesse;
    } else {
        /* On met les directions vers l'avant */
        PORTB &=~ (1<<2);
    }

    /* On active la vitesse pour roue de droite seulement */
    OCR0A = (vitesse*255)/100;
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
    OCR0B = (vitesse*255)/100;
}

void requete_info(uint8_t identification){
    
    if(identification == 0x00){
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
			case 0xf8: 
            /*Le robot exécutera une rotation à la roue gauche */
			moteur_controller_gauche(uart_receive());
			break;
			case 0xf9:
            /*Le robot exécutera une rotation à la roue droite */
			moteur_controller_droite(uart_receive());
			break;
			case 0xfa:
            /*Le robot changera la couleur de sa del libre */
			couleur_del(uart_receive());
			break;
			case 0xfb:
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

