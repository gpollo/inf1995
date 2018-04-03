
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

void init_interruption(void) {
	
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

volatile uint8_t boutton = 1;
ISR(INT0_vect) {
    cli();
    boutton = (boutton == 0) ? 1 : 0;
    
    uart_putchar(0xf5);
    uart_putchar(boutton);
    _delay_ms(30);
    EIFR |= (1 << INTF0) ;
    sei();
}

/* Distance en cm détectée par le capteur gauche (1 octet) */
void distance_gauche(void) {

}

/* Distance en cm détectée par le capteur droite (1 octet) */
void distance_droite(void) {
	
}

void vitesse_gauche(uint8_t vitesse1){
	
	moteur_controller_gauche();
}

void vitesse_droite(uint8_t vitesse2){
	moteur_controller_droite();
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

void requete_info(uint8_t identification){
	
}

void listen(void) {
	while(1) {
		switch(uart_receive()) {
			case 0xf8:
			moteur_controller_gauche(uart_receive());
			break;
			case 0xf9:
			moteur_controller_droite(uart_receive());
			break;
			case 0xfa:
			couleur_del(uart_receive());
			break;
			case 0xfb:
			break;
		}
	}
}

int main(void) {
	uart_init();
	del_init();
	moteur_init();
	nom_robot();
    numero_equipe();
    numero_section();
    session();
    couleur_base();	
    init_interruption();
    listen();

}

