#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>




int main()
{
	DDRA = 0xff;
	DDRB = 0xff;
	DDRC = 0xff;
	DDRD = 0x00;
	
	PORTB=0x00;	
	
	int a = 0;
	
	while((a>=0)&&(a<2000))	
{
		
		PORTB=0x01;
		_delay_ms(0);
		PORTB=0x00;
		_delay_ms(2.5);
		a++;
	}
	
	while((a>=2000)&&(a<4000))	
	{
		
		PORTB=0x01;
		_delay_ms(0.625);
		PORTB=0x00;
		_delay_ms(1.875);
		a++;
	}
	
	while((a>=4000)&&(a<6000))	
	{
		
		PORTB=0x01;
		_delay_ms(1.250);
		PORTB=0x00;
		_delay_ms(1.250);
		a++;
	}
	
	while((a>=6000)&&(a<8000))	
	{
		
		PORTB=0x01;
		_delay_ms(1.875);
		PORTB=0x00;
		_delay_ms(0.625);
		a++;
	}
	
	while((a>=8000)&&(a<10000))	
	{
		
		PORTB=0x01;
		_delay_ms(2.5);
		PORTB=0x00;
		_delay_ms(0);
		a++;
	}
	
}	

