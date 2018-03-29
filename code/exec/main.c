
#include <util/delay.h>
#include <memory.h>
#include <uart.h>


void memory_test(void) {
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
  
int main(void) {
    uart_init();
    memory_test();
}
