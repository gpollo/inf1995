#define F_CPU 8000000UL

#include <avr/io.h> 
#include <util/delay.h>
#define DELAY 500
int main()
{
  DDRB = 0xff;
  for(;;)  // boucle sans fin
  {
  	// Alternance 01 et 10
	
//	DDRB = 0x01;

	PORTB = 0x01;
	_delay_ms(DELAY);

//  	DDRB = 0x02;

 	PORTB = 0x02;
	_delay_ms(DELAY); 
  }
  return 0; 
}

