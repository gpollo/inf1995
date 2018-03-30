
#include <uart.h>
#include <memory.h>
#include <timer.h>
#include <adc.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <del.h>
//#include <sensor.h>



void identification(void) {
    memory_init();
	
	/* On écrit le nom du robot */
	uart_putchar(0xf0);
    uart_printf("FireFightingB");
   
    /* On écrit le numéro de l'équipe (9 octets) */
	uart_putchar(0xf1);
    uart_printf("6266     ");

	
	/* On écrit le groupe (1 octet) */
	uart_putchar(0xf2);
	
	uart_putchar(3);


	/* On écrit la session (4 octets)*/
	uart_putchar(0xf3);
    uart_printf("18-1");
   
 
    /* On écrit la couleur de la base (1 octet) */
    uart_putchar(0xf4);
    uart_putchar('1');
 
}
	
	/** Compteur pour le premier callback. */
	volatile uint8_t counter1 = 5;

	/** Compteur pour le deuxième callback. */
	volatile uint8_t counter2 = 5;	

	/** Compteur pour le troisième callback. */
	volatile uint8_t counter3 = 5;

	/** Premier callback. */
	void etat_interrupt(void);

	/** Premier callback. */
	void distance_gauche(void);

	/** Premier callback. */
	void distance_droite(void);

	/** Structure pour la configuration du premier callback. */
	struct callback callback1 = {
		.time = 1000,
		.func = &etat_interrupt,
		.repeat = 1,
	};

	/** Structure pour la configuration du deuxième callback. */
	struct callback callback2 = {
		.time = 2500,
		.func = &distance_gauche,
		.repeat = 1,
	};

	/** Structure pour la configuration du troisième callback. */
	struct callback callback3 = {
		.time = 1500,
		.func = &distance_droite,
		.repeat = 1,
	};

void timer_test(void) {
    timer_init();

    /* on démarre les deux premiers timers */
    timer_start(&callback1, &callback2);

    while(callback1.repeat || callback2.repeat || callback3.repeat)
        /* on démarre le troisième quand le premier finit */
        if(!counter1) timer_start(&callback3, &CALLBACK_IGNORE);
}

	/* État du bouton Interrupt (1 octet) */
void etat_interrupt(void) {
	uart_putchar(0xf5);
	DDRD = 0x00;    
	 while (1) {
	/*Si le bouton est appuyé, interrupt ___ */
	  if (PIND & 0x04 ) {
		_delay_ms (10);
		uart_putchar(0x00);
	}
	/*S'il n'est pas appuyé, interrupt _/_ */
	else
	uart_putchar(0x01);
  }
	if(!counter1) callback1.repeat = 0;
}

	/* Distance en cm détectée par le capteur gauche (1 octet) */
void distance_gauche(void) {
	uart_putchar(0xf6);
//	 while (1) {
//       uint8_t data = memory_read_byte(0, addr++);
//        uart_printf("0x%02x\t", data);
//		sensor_read(&capteurs);
//        int16_t data = distance_value(capteurs.gauche);
//        uart_printf("%i\t", data);
//         _delay_ms(500);
//	    sensor_read(&capteur);

 
//     }
    if(!counter2) callback2.repeat = 0;
}
/* Distance en cm détectée par le capteur droite (1 octet) */
void distance_droite(void) {
	uart_putchar(0xf7);
//	uart_putchar();
    if(!counter3) callback3.repeat = 0;
}
  
int main(void) {
	uart_init();
    identification();
    timer_test();
   
    etat_interrupt();
// 	  adc_init();
//    distance_gauche();
//    distance_droite();
}

