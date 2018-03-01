
#include <avr/io.h> 
#define F_CPU 8000000
#include <util/delay.h>

void delayB(int a)
{
	for(int i=0; i < a; i++)
	{
	_delay_us(1);
	}

}


int main ()
{
	DDRA = 0xff;
	DDRB = 0xff;
	DDRC = 0xff;
	DDRD = 0xff;
	
	
	
	
	
		/*/for( int i=0; i<3000; i++)
		{
		PORTB =0x01;
		int b = 3000-i;
		delay(b);
		
		PORTB = 0x00;
		delay(3000);
		}
		/*/
		for( int i=0; i<3; i++)
		{
			for(int j = 0; j < 1000; j++)
			{
		PORTB =0x01;
		int b = 1000-j;
		delayB(b);
		
		PORTB = 0x00;
		delayB(j);
		
		}
	
		delay(i);
	}
		
	
		
}
	
	
	
	
	
	
	
	
	
	
	
	

